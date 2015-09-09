<?hh


  namespace HCMS\Hooks\Cron;

  /**
   * Class ProcessBackups
   * @package HC\Hooks\Cron
   */

  class ProcessBackups extends \HC\Hooks\Cron

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
          echo 'Processing Backups' . PHP_EOL;
          $db = new \HC\DB();
          $result = $db->query('SELECT `D`.`id`,`D`.`title`, `D`.`backupType`, `D`.`backupInterval`, `D`.`lastBackUp` FROM `databases` `D` WHERE `D`.`status` = 1 AND `D`.`backupType` != 0 AND `D`.`backupInterval` > 0');
          if($result) {
              $time = time();
              foreach($result as $row) {
                  $checkTime = ($time - ($row['backupInterval'] * 3600));

                  if ($checkTime >= $row['lastBackUp']) {
                      echo 'Processing: ' . $row['title'] . PHP_EOL;
                      $before = microtime(true);
                      $dateTokens = explode('.', $before);
                      if(!isset($dateTokens[1])) {
                          $dateTokens[1] = 0;
                      }

                      $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                      $status = $db->write('database_backups', ['databaseID' =>  $row['id'], 'status' => 1, 'isLocal' => 1, 'isAuto' => 1, 'progress' => 0, 'dateCreated' => $dateCreated, 'dateEdited' => $dateCreated]);
                      if($status) {
                          $status = $db->update('databases', ['id' => $row['id']], ['lastBackUp' => $time]);
                          if(!$status) {
                              throw new \Exception('Unable to write to database');
                          }
                      } else {
                          throw new \Exception('Unable to write to database');
                      }
                  } else {
                      echo 'Skipped: ' . $row['title'] . PHP_EOL;
                  }
              }

              echo 'Processed Backups' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }
  }
