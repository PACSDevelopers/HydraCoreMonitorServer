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
          ], ['D.id' => 'domainID', 'S.id' => 'serverID', 'S.hasIssue' => 'hasIssue', 'D.title' => 'domainTitle', 'S.title' => 'serverTitle', 'D.url', 'S.ip'], ['S.status' => 1,'D.status' => 1]);
          
          if($result) {
              $before = microtime(true);
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              if(isset($settings['domain']) && isset($settings['key'])) {
                  $authenticator = new \HC\Authenticator();
                  $authenticator->setCodeLength(9);
              }
              
              $servers = [];
             
              foreach($result as $row) {
                  $isUniqueServer = true;
                  
                  if(isset($servers[$row['serverID']])) {
                      $isUniqueServer = false;
                  }
                  
                  $extraData = [
                      'redirect_count' => 0,
                      'redirect_time' => 0,
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

                  if($isValidConnection === 503) {
                      if(isset($errorDetails['status'])) {
                          if($errorDetails['status'] === '503-2') {
                              $isValidConnection = 200;
                          }
                      }
                  }


                  if(isset($extraData['total_time']) && isset($extraData['redirect_time'])) {
                      $after = ($extraData['total_time'] - $extraData['redirect_time']);
                  }
                  
                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                  if($isValidConnection === 200) {
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Passed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  } else {
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                      $after = 0;
                  }

                  $currentClientData = ['status' => $isValidConnection, 'serverID' => $row['serverID'], 'domainID' => $row['domainID'], 'responseTime' => $after, 'dateCreated' => $dateCreated, 'cpu' => 0, 'mem' => 0, 'iow' => 0, 'ds' => 0, 'net' => 0, 'rpm' => 0, 'tps' => 0, 'avgRespTime' => 0, 'qpm' => 0, 'avgTimeCpuBound' => 0];
                  
                  if(isset($settings['domain']) && isset($settings['key']) && $isUniqueServer) {
                      $tempClientData = \HCMS\Server::checkClient(long2ip($row['ip']), $settings['domain'], 'http', '/v1/all/get?code=' . $authenticator->getCode($settings['key']));
                      
                      if($tempClientData && isset($tempClientData['result'])) {
                          if(isset($tempClientData['result']['updates']) && isset($tempClientData['result']['securityUpdates']) && isset($tempClientData['result']['rebootRequired'])) {
                              $db->update('servers', ['id' => $row['serverID']], ['updates' => $tempClientData['result']['updates'], 'securityUpdates' => $tempClientData['result']['securityUpdates'], 'rebootRequired' => $tempClientData['result']['rebootRequired']]);
                          }

                          $currentClientData['cpu']             = $tempClientData['result']['cpu'];
                          $currentClientData['mem']             = $tempClientData['result']['mem'];
                          $currentClientData['iow']             = $tempClientData['result']['iow'];
                          $currentClientData['ds']              = $tempClientData['result']['ds'];
                          $currentClientData['net']             = $tempClientData['result']['net'];
                          $currentClientData['rpm']             = $tempClientData['result']['rpm'];
                          $currentClientData['tps']             = $tempClientData['result']['tps'];
                          $currentClientData['avgRespTime']     = $tempClientData['result']['avgRespTime'];
                          $currentClientData['qpm']             = $tempClientData['result']['qpm'];
                          $currentClientData['avgTimeCpuBound'] = $tempClientData['result']['avgTimeCpuBound'];
                      }
                      
                      
                      
                      $db->write('server_history', $currentClientData);
                      $servers[$row['serverID']] = $currentClientData;
                  } else {
                      $currentClientData = $servers[$row['serverID']];
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

                          $error = [
                              'status'       => $isValidConnection,
                              'message'      => $errorDetails['message'],
                              'description'  => $errorDetails['errorDescription'],
                          ];

                          if(isset($errorDetails['errorDetails']['Error Number'])) {
                              $error['errorCode'] = $errorDetails['errorDetails']['Error Number'];
                          }

                          if(isset($errorDetails['errorDetails']['Error Message'])) {
                              $error['errorMessage'] = $errorDetails['errorDetails']['Error Message'];
                          }

                          if(isset($errorDetails['errorDetails']['ID'])) {
                              $error['hash'] = $errorDetails['errorDetails']['ID'];
                          } else {
                              $error['hash'] = crc32(serialize($error));
                          }

                          $errors = $db->read('errors', ['id'], ['hash' => $error['hash']]);
                          if($errors) {
                              $errorID = $errors[0]['id'];
                          } else {
                              $error['details'] = lz4_compress(serialize($errorDetails['errorDetails']));
                              $error['dateCreated'] = time();
                              $error = \HCMS\Error::create($error);
                              $errorID = $error->id;
                          }
                      } else {
                          $errorID = null;
                          \HCMS\Server::alertDown($data);
                      }

                      // Check if issue exists for this error
                      $result = $db->query('SELECT `I`.`id` FROM `issues` `I` WHERE `I`.`serverID` = ? AND `I`.`domainID` = ? AND `I`.`errorID` = ? AND `I`.`auto` = ? AND `I`.`status` IN (1,2);', [$row['serverID'], $row['domainID'], $errorID, 1]);
                      if($result) {
                          // Confirm it
                          $db->update('issues', ['id' => $result[0]['id']], ['dateLastConfirmed' => time()]);
                      } else {
                          // Create it
                          $time = time();
                          $issue = \HCMS\Issue::create([
                              'serverID'     => $row['serverID'],
                              'domainID'     => $row['domainID'],
                              'errorID'      => $errorID,
                              'status'       => 1,
                              'dateCreated'  => $time,
                              'dateLastConfirmed' => $time,
                              'auto'         => 1
                          ]);

                          $db->update('servers', ['id' => $row['serverID']], ['hasIssue' => 1]);
                      }
                  } else {
                      // Resolve any auto issues
                      $result = $db->query('SELECT `I`.`id` FROM `issues` `I` WHERE `I`.`serverID` = ? AND `I`.`domainID` = ? AND `I`.`auto` = ? AND `I`.`status` IN (1,2);', [$row['serverID'], $row['domainID'], 1]);
                      if($result) {
                          $db->beginTransaction();
                          foreach($result as $issue) {
                              $db->update('issues', ['id' => $issue['id']], ['status' => 3, 'dateClosed' => time()]);
                          }
                          $db->update('servers', ['id' => $row['serverID']], ['hasIssue' => 0]);
                          $db->commit();
                      } else if($row['hasIssue']) {
                          $db->update('servers', ['id' => $row['serverID']], ['hasIssue' => 0]);
                      }
                  }
              }

              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              $db->query('DELETE FROM `server_history` WHERE `dateCreated` < ?;', [$dateCreated]);

              echo 'Processed Servers' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
