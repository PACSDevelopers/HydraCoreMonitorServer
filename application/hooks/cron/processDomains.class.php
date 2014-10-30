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
          $result = $db->read('domains', ['id', 'title', 'url'], ['status' => 1]);
          if($result) {
              $before = microtime(true);
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

              $overview = ['up' => 0, 'dateCreated' => $dateCreated, 'responseTime' => []];
              
              $db->beginTransaction();

              foreach($result as $row) {
                  $extraData = [
                      'redirect_count' => 0,
                      'redirect_time' => 0,
                      'ssl_verify_result' => 999999,
                  ];
                  
                  $before = microtime(true);
                  $isValidConnection = \HCMS\Domain::checkHTTP($row['url'], true, $extraData);
                  $after = microtime(true) - $before;
                  
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
                      $overview['up']++;
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
                      
                      \HCMS\Domain::alertDown($data);
                  }
                  $overview['responseTime'][] = $after;
                  
                  $db->write('domain_history', ['status' => $isValidConnection, 'domainID' => $row['id'], 'responseTime' => $after, 'dateCreated' => $dateCreated]);
              }

              $overview['responseTime'] = array_sum($overview['responseTime']) / count($overview['responseTime']);

              $overview['percent'] = ($overview['up'] / count($result)) * 100;

              unset($overview['up']);
              $db->write('domain_history_overview', $overview);

              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

              $db->query('DELETE FROM `domain_history` WHERE `dateCreated` < ?;', [$dateCreated]);
              $db->query('DELETE FROM `domain_history_overview` WHERE `dateCreated` < ?;', [$dateCreated]);
              
              $db->commit();

              echo 'Processed Domains' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
