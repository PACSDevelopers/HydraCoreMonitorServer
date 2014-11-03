<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessDatabases
   * @package HC\Hooks\Cron
   */

  class ProcessDatabases extends \HC\Hooks\Cron

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
          $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
          
          if(is_array($settings)) {
              $this->settings = $settings;
          }
      }



      /**
       * @return bool
       */

      public function run()

      {
          echo 'Processing Databases' . PHP_EOL;
          
          $db = new \HC\DB();
          $result = $db->read('databases', ['id', 'title', 'ip'], ['status' => 1]);
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
                  $before = microtime(true);
                  $isValidConnection = \HCMS\Database::testMySQLPort(long2ip($row['ip']));
                  $after = microtime(true) - $before;
                  
                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                  if($isValidConnection) {
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Passed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                      $overview['up']++;
                  } else {
                      $data = [
                              'Code'                    => $isValidConnection,
                              'Date'                    => $dateCreated,
                              'Database Title'          => $row['title'],
                              'Database ID'             => $row['id'],
                              'Time Elapsed'            => $after,
                              'IP Address'              => <a href={'http://' . long2ip($row['ip'])}>{long2ip($row['ip'])}</a>,
                              'Date'                    => $dateCreated,
                      ];
                      
                      \HCMS\Database::alertDown($data);
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  }
                  $overview['responseTime'][] = $after;
                  
                  $db->write('database_history', ['status' => $isValidConnection, 'databaseID' => $row['id'], 'responseTime' => $after, 'dateCreated' => $dateCreated]);
              }

              $overview['responseTime'] = array_sum($overview['responseTime']) / count($overview['responseTime']);

              $overview['percent'] = ($overview['up'] / count($result)) * 100;

              unset($overview['up']);
              $db->write('database_history_overview', $overview);


              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

              $db->query('DELETE FROM `database_history` WHERE `dateCreated` < ?;', [$dateCreated]);
              $db->query('DELETE FROM `database_history_overview` WHERE `dateCreated` < ?;', [$dateCreated]);
              
              $db->commit();

              echo 'Processed Databases' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
