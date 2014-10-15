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
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Passed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                      $overview['up']++;
                  } else {
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Failed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                  }
                  $overview['responseTime'][] = $after;
                  $db->write('domain_history', ['domainID' => $row['id'], 'status' => $isValidConnection, 'responseTime' => $after, 'dateCreated' => $dateCreated, 'redirects' => $extraData['redirect_count'], 'redirectTime' => $extraData['redirect_time'], 'ssl' => $extraData['ssl_verify_result']]);
              }

              $overview['responseTime'] = array_sum($overview['responseTime']) / count($overview['responseTime']);

              $overview['percent'] = ($overview['up'] / count($result)) * 100;

              unset($overview['up']);
              $db->write('domain_history_overview', $overview);
              
              $db->commit();

              echo 'Processed Domains' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
