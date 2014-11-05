<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessCleanup
   * @package HC\Hooks\Cron
   */

  class ProcessCleanup extends \HC\Hooks\Cron

  {
      
      /**
       * @var bool
       */

      protected $settings = [
          'archive' => '/data/archive'
      ];



      /**
       * @param bool $settings
       */

      public function __construct($settings = [])

      {
          $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
          if(isset($globalSettings['backups'])) {
              $this->settings = $this->parseOptions($this->settings, $globalSettings['backups']);
          } 
          
          if(is_array($settings)) {
              $this->settings = $settings = $this->parseOptions($this->settings, $settings);
          }
      }



      /**
       * @return bool
       */

      public function run()

      {
          echo 'Processing Cleanup' . PHP_EOL;
          $df = disk_free_space($this->settings['archive']);
          $dt = disk_total_space($this->settings['archive']);
          $ds = 100 - ($df / $dt) * 100;

          if($ds > 85) {
              echo 'Running Cleanup' . PHP_EOL;
              $db = new \HC\DB();
              $result = $db->query('SELECT `D`.`id` FROM `database_backups` `D` WHERE (`D`.`status` = 3 AND `D`.`inVault` = 1 AND `D`.`isLocal` = 1) ORDER BY `D`.`dateEdited` DESC LIMIT 0, 10;');
              if($result) {
                  foreach($result as $row) {
                      echo 'Deleted: ' . $row['id'] . PHP_EOL;
                      
                      if(is_file($this->settings['archive'] . '/' . $row['id'] . '.tar.xz')) {
                          unlink($this->settings['archive'] . '/' . $row['id'] . '.tar.xz');
                      }

                      $before = microtime(true);
                      $dateTokens = explode('.', $before);
                      if(!isset($dateTokens[1])) {
                          $dateTokens[1] = 0;
                      }

                      $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                      
                      $db->update('database_backups', ['id' => $row['id'], ['isLocal' => 0, 'dateEdited' => $dateEdited]]);
                  }
                  echo 'Processed Cleanup' . PHP_EOL;
                  return true;
              } else {
                  echo 'Processed Cleanup' . PHP_EOL;
                  return true;
              }
          } else {
              echo 'Processed Cleanup' . PHP_EOL;
              return true;
          }
          
          return false;

      }
  }
