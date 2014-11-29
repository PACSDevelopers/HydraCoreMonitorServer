<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessDomains
   * @package HC\Hooks\Cron
   */

  class ProcessDomains extends \HC\Hooks\Cron

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
          echo 'Processing Domains' . PHP_EOL;

          $db = new \HC\DB();
          $result = $db->query('SELECT 
                                    `D`.`id`, `D`.`title`, `D`.`url`
                                FROM
                                    `domains` `D`
                                LEFT JOIN `server_mapping` `SM` ON (`SM`.`domainID` = `D`.`id`)
                                LEFT JOIN `servers` `S` ON (`S`.`id` = `SM`.`serverID`)
                                WHERE
                                    `D`.`status` = 1
                                AND
                                    (`SM`.`serverID` IS NULL || `S`.`status` = 0);');
          if($result) {
              $settings = [];
              $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
              if(isset($globalSettings['monitor-server'])) {
                  $settings = $globalSettings['monitor-server'];
              }
              
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

              foreach($result as $row) {
                  $extraData = [
                      'redirect_count' => 0,
                      'redirect_time' => 0,
                      'ssl_verify_result' => 999999,
                  ];


                  $key = false;
                  $auth = false;
                  if(isset($settings['domain']) && isset($settings['key'])) {
                      $key = $settings['key'];
                      $auth = $authenticator;
                  }
                  
                  $errorDetails = [];
                  
                  $before = microtime(true);
                  $isValidConnection = \HCMS\Domain::checkHTTP($row['url'], true, $extraData, $errorDetails, $key, $auth);
                  $after = microtime(true) - $before;

                  if($isValidConnection === 503) {
                      if(isset($errorDetails['status'])) {
                          if($errorDetails['status'] === '503-2') {
                              $isValidConnection = 200;
                          }
                      }
                  }
                  
                  if(isset($extraData['total_time'])) {
                      $after = $extraData['total_time'];
                  }
                  
                  
                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                  
                  switch($extraData['ssl_verify_result']) {
                      case 999999:
                          $extraData['ssl_verify_result'] = 0;
                      break;
                      
                      case 0:
                          $extraData['ssl_verify_result'] = 1;
                      break;
                  }
                  
                  if($isValidConnection === 200) {
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Passed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  } else {
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Failed in ' . $after . 'ms  on' . $dateCreated . PHP_EOL;
                      $data = [
                              'Code'                    => $isValidConnection,
                              'Date'                    => $dateCreated,
                              'Domain Title'            => $row['title'],
                              'Domain ID'               => $row['id'],
                              'Time Elapsed'            => $after,
                              'URL'                     => <a href={'http://' . $row['url']}>{$row['url']}</a>,
                              'Date'                    => $dateCreated,
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

                              \HCMS\Domain::alertDown($data);
                          }
                      } else {
                          \HCMS\Domain::alertDown($data);
                      }
                  }
                  
                  $db->write('domain_history', ['status' => $isValidConnection, 'domainID' => $row['id'], 'responseTime' => $after, 'dateCreated' => $dateCreated]);
              }

              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

              $db->query('DELETE FROM `domain_history` WHERE `dateCreated` < ?;', [$dateCreated]);

              echo 'Processed Domains' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
