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
              
              $db->beginTransaction();

              foreach($result as $row) {
                  $extraData = [
                      'redirect_count' => 0,
                  ];

                  $before = microtime(true);
                  $isValidConnection = \HCMS\Server::checkHTTP(long2ip($row['ip']), $row['url'], true, $extraData);
                  $after = microtime(true) - $before;

                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                  if($isValidConnection === 200) {
                      $overview['up']++;
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Passed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                  } else {
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Failed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                  }

                  $overview['responseTime'][] = $after;

                  $currentClientData = ['cpu' => 0, 'mem' => 0, 'iow' => 0, 'ds' => 0, 'net' => 0, 'rpm' => 0, 'tps' => 0, 'avgRespTime' => 0, 'qpm' => 0, 'avgTimeCpuBound' => 0];
                  
                  if(isset($settings['domain']) && isset($settings['key'])) {
                      $before2 = microtime(true);
                      $tempClientData = \HCMS\Server::checkClient(long2ip($row['ip']), $settings['domain'], 'http', '/v1/all/get?code=' . $authenticator->getCode($settings['key']));
                      $after2 = microtime(true) - $before2;
                      var_dump($after2);
                      if($tempClientData && isset($tempClientData['result'])) {
                          var_dump($tempClientData);
                          $currentClientData = $tempClientData['result'];
                          $overview['cpu'][] = $tempClientData['result']['cpu'];
                          $overview['mem'][] = $tempClientData['result']['mem'];
                          $overview['iow'][] = $tempClientData['result']['iow'];
                          $overview['ds'][] = $tempClientData['result']['ds'];
                          $overview['net'][] = $tempClientData['result']['net'];
                          $overview['rpm'][] = $tempClientData['result']['rpm'];
                          $overview['tps'][] = $tempClientData['result']['tps'];
                          $overview['avgRespTime'][] = $tempClientData['result']['avgRespTime'];
                          $overview['qpm'][] = $tempClientData['result']['qpm'];
                          $overview['avgTimeCpuBound'][] = $tempClientData['result']['avgTimeCpuBound'];
                      }
                  }
                  
                  $db->write('server_history', [
                      'domainID' => $row['domainID'],
                      'serverID' => $row['serverID'],
                      'status' => $isValidConnection,
                      'responseTime' => $after,
                      'cpu' => $currentClientData['cpu'],
                      'mem' => $currentClientData['mem'],
                      'iow' => $currentClientData['iow'],
                      'ds' => $currentClientData['ds'],
                      'net' => $currentClientData['net'],
                      'rpm' => $currentClientData['rpm'],
                      'tps' => $currentClientData['tps'],
                      'avgRespTime' => $currentClientData['avgRespTime'],
                      'qpm' => $currentClientData['qpm'],
                      'avgTimeCpuBound' => $currentClientData['avgTimeCpuBound'],
                      'dateCreated' => $dateCreated,
                      'redirects' => $extraData['redirect_count']
                  ]);
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
              
              $db->commit();

              echo 'Processed Servers' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
