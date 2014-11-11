<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessServers
   * @package HC\Hooks\Cron
   */

  class ProcessServers extends \HC\Hooks\Cron

  {



      /**
       * @var bool
       */

      protected $settings = [];



      /**
       * @param bool $settings
       */

      public function __construct($settings = [])

      {
          if(is_array($settings)) {
              $this->settings = $settings;
          }
      }



      /**
       * @return bool
       */

      public function run()

      {
          echo 'Processing Servers' . PHP_EOL;
            
          $settings = [];
          $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
          if(isset($globalSettings['monitor-server'])) {
              $settings = $globalSettings['monitor-server'];
          }
          
          $db = new \HC\DB();
          $db->query('SET SESSION innodb_lock_wait_timeout = 300;');
          
          $result = $db->read([
              'servers' => 'S',
              'J.SM.server_mapping' => [
                  'SM.serverID' => 'S.id'
              ],
              'J.D.domains' => [
                  'D.id' => 'SM.domainID'
              ]
          ], ['D.id' => 'domainID', 'S.id' => 'serverID', 'D.title' => 'domainTitle', 'S.title' => 'serverTitle', 'D.url', 'S.ip'], ['S.status' => 1,'D.status' => 1]);
          
          
          if($result) {
              $before = microtime(true);
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              $overview = ['up' => 0, 'dateCreated' => $dateCreated, 'responseTime' => [], 'cpu' => [], 'mem' => [], 'iow' => [], 'ds' => [], 'net' => [], 'rpm' => [], 'tps' => [], 'avgRespTime' => [], 'qpm' => [], 'avgTimeCpuBound' => []];

              if(isset($settings['domain']) && isset($settings['key'])) {
                  $authenticator = new \HC\Authenticator();
                  $authenticator->setCodeLength(9);
              }
              
              $servers = [];
              
              $db->beginTransaction();

             
              foreach($result as $row) {
                  $isUniuqeServer = true;
                  
                  if(isset($servers[$ip])) {
                      $isUniuqeServer = false;
                  } else {
                      $servers[$ip] = true;
                  }
                  
                  $extraData = [
                      'redirect_count' => 0,
                  ];

                  $key = false;
                  $auth = false;

                  if(isset($settings['domain']) && isset($settings['key'])) {
                      $key = $settings['key'];
                      $auth = $authenticator;
                  }

                  $errorDetails = [];
                  
                  $before = microtime(true);
                  $isValidConnection = \HCMS\Server::checkHTTP(long2ip($row['ip']), $row['url'], true, $extraData, $errorDetails, $key, $auth);
                  $after = microtime(true) - $before;

                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                  if($isValidConnection === 200) {
                      $overview['up']++;
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Passed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  } else {
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  }

                  $overview['responseTime'][] = $after;

                  $currentClientData = ['status' => $isValidConnection, 'serverID' => $row['serverID'], 'domainID' => $row['domainID'], 'responseTime' => $after, 'dateCreated' => $dateCreated, 'cpu' => 0, 'mem' => 0, 'iow' => 0, 'ds' => 0, 'net' => 0, 'rpm' => 0, 'tps' => 0, 'avgRespTime' => 0, 'qpm' => 0, 'avgTimeCpuBound' => 0];
                  
                  if(isset($settings['domain']) && isset($settings['key']) && $isUniuqeServer) {
                      $ip = long2ip($row['ip']);
                      
                      $before2 = microtime(true);
                      $tempClientData = \HCMS\Server::checkClient($ip, $settings['domain'], 'http', '/v1/all/get?code=' . $authenticator->getCode($settings['key']));
                      $after2 = microtime(true) - $before2;
                      if($tempClientData && isset($tempClientData['result'])) {
                          
                          if(isset($tempClientData['result']['updates']) && isset($tempClientData['result']['securityUpdates']) && isset($tempClientData['result']['rebootRequired'])) {
                              $db->update('servers', ['id' => $row['serverID']], ['updates' => $tempClientData['result']['updates'], 'securityUpdates' => $tempClientData['result']['securityUpdates'], 'rebootRequired' => $tempClientData['result']['rebootRequired']]);
                          }
                          
                          $overview['cpu'][]             = $currentClientData['cpu']             = $tempClientData['result']['cpu'];
                          $overview['mem'][]             = $currentClientData['mem']             = $tempClientData['result']['mem'];
                          $overview['iow'][]             = $currentClientData['iow']             = $tempClientData['result']['iow'];
                          $overview['ds'][]              = $currentClientData['ds']              = $tempClientData['result']['ds'];
                          $overview['net'][]             = $currentClientData['net']             = $tempClientData['result']['net'];
                          $overview['rpm'][]             = $currentClientData['rpm']             = $tempClientData['result']['rpm'];
                          $overview['tps'][]             = $currentClientData['tps']             = $tempClientData['result']['tps'];
                          $overview['avgRespTime'][]     = $currentClientData['avgRespTime']     = $tempClientData['result']['avgRespTime'];
                          $overview['qpm'][]             = $currentClientData['qpm']             = $tempClientData['result']['qpm'];
                          $overview['avgTimeCpuBound'][] = $currentClientData['avgTimeCpuBound'] = $tempClientData['result']['avgTimeCpuBound'];
                      }
                      
                      $db->write('server_history', $currentClientData);
                  }

                  if($isValidConnection !== 200) {
                      $data = [
                          'Code'                    => $isValidConnection,
                          'Date'                    => $dateCreated,
                          'Server Title'            => $row['serverTitle'],
                          'Server ID'               => $row['serverID'],
                          'Domain Title'            => $row['domainTitle'],
                          'Domain ID'               => $row['domainID'],
                          'Time Elapsed'            => $after,
                          'URL'                     => <a href={'http://' . $row['url']}>{$row['url']}</a>,
                          'IP Address'              => <a href={'http://' . long2ip($row['ip'])}>{long2ip($row['ip'])}</a>,
                          'Date'                    => $dateCreated,
                          'CPU'                     => $currentClientData['cpu'],
                          'Memory'                  => $currentClientData['mem'],
                          'IO Wait'                 => $currentClientData['iow'],
                          'Disk Space'              => $currentClientData['ds'],
                          'Network Traffic'         => $currentClientData['net'],
                          'Requests Per Minute'     => $currentClientData['rpm'],
                          'Transactions Per Second' => $currentClientData['tps'],
                          'Average Response Time'   => $currentClientData['avgRespTime'],
                          'Queries Per Minute'      => $currentClientData['qpm'],
                          'Average Time CPU Bound'  => $currentClientData['avgTimeCpuBound'],
                      ];
                      
                      if(isset($errorDetails['status'])) {
                          // Only send notifications if it's not a deployment
                          if($errorDetails['status'] !== '503-2') {
                              $data['Error Status'] = $errorDetails['status'];
                              $data['Error Message'] = $errorDetails['message'];
                              $data['Error Description'] = $errorDetails['errorDescription'];
                              foreach($errorDetails['errorDetails'] as $key => $value) {
                                  if(is_array($value)) {
                                      $tempVal = <small></small>;
                                      foreach($value as $key2 => $value2) {
                                          $tempVal->appendChild(<x:frag>[{$key2}]{$value2}<br /></x:frag>);
                                      }
                                      $data['Error Details ' . $key] = $tempVal;
                                  } else {
                                      $data['Error Details ' . $key] = $value;
                                  }
                              }
                              
                              \HCMS\Server::alertDown($data);
                          }
                      } else {
                          \HCMS\Server::alertDown($data);
                      }
                  }
              }
            
              $serverCount = count($result);
              if($serverCount) {
                  $overview['responseTime'] = array_sum($overview['responseTime']) / $serverCount;
                  $overview['percent'] = ($overview['up'] / $serverCount) * 100;
              } else {
                  $overview['responseTime'] = 0;
                  $overview['percent'] = 0;
              }
              
              $clientCount = count($overview['cpu']);
              if($clientCount) {
                  $overview['cpu'] = array_sum($overview['cpu']) / $clientCount;
                  $overview['mem'] = array_sum($overview['mem']) / $clientCount;
                  $overview['iow'] = array_sum($overview['iow']) / $clientCount;
                  $overview['ds'] = array_sum($overview['ds']) / $clientCount;
                  $overview['net'] = array_sum($overview['net']) / $clientCount;
                  $overview['rpm'] = array_sum($overview['rpm']);
                  $overview['tps'] = array_sum($overview['tps']) / $clientCount;
                  $overview['avgRespTime'] = array_sum($overview['avgRespTime']) / $clientCount;
                  $overview['qpm'] = array_sum($overview['qpm']);
                  $overview['avgTimeCpuBound'] = array_sum($overview['avgTimeCpuBound']) / $clientCount;
              } else {
                  $overview['cpu'] = 0;
                  $overview['mem'] = 0;
                  $overview['iow'] = 0;
                  $overview['ds'] = 0;
                  $overview['net'] = 0;
                  $overview['rpm'] = 0;
                  $overview['tps'] = 0;
                  $overview['avgRespTime'] = 0;
                  $overview['qpm'] = 0;
                  $overview['avgTimeCpuBound'] = 0;
              }
                
              unset($overview['up']);
              
              $db->write('server_history_overview', $overview);

              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              $db->query('DELETE FROM `server_history` WHERE `dateCreated` < ?;', [$dateCreated]);
              $db->query('DELETE FROM `server_history_overview` WHERE `dateCreated` < ?;', [$dateCreated]);
              $db->commit();

              echo 'Processed Servers' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
