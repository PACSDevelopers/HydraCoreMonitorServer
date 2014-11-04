<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessBackups
   * @package HC\Hooks\Cron
   */

  class ProcessManualBackups extends \HC\Hooks\Cron

  {



      /**
       * @var bool
       */

      protected $settings = [
          'path' => '/data/archive'
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
          echo 'Processing Manual Backups' . PHP_EOL;
          $encryption = new \HC\Encryption();
          $db = new \HC\DB();
          $result = $db->query('SELECT `DB`.`id` as `backupID`, `D`.`id`, `D`.`title`, `D`.`ip`, `D`.`username`, `D`.`password`, `D`.`backupType`, `D`.`dateCreated` FROM `database_backups` `DB` LEFT JOIN `databases` `D` ON (`D`.`id` = `DB`.`databaseID`) WHERE `DB`.`status` = 1;');
          if($result) {
              foreach($result as $row) {
                  echo 'Processing Database Backup: ' . $row['backupID'] . ' - ' . $row['title']  . ' (' . $row['id'] . ')' . PHP_EOL;
                  $rowResult = $db->read('database_backups', ['status'], ['id' => $row['backupID']]);
                  if($rowResult[0]['status'] == 1) {
                      $before = microtime(true);
                      $dateTokens = explode('.', $before);
                      if(!isset($dateTokens[1])) {
                          $dateTokens[1] = 0;
                      }

                      $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                      
                      $status = $db->update('database_backups', ['id' => $row['backupID'], 'status' => 1], ['status' => 2, 'dateEdited' => $dateEdited],);
                      if($status) {
                          $row['username'] = $encryption->decrypt($row['username'], 'HC_DB_U' . $row['dateCreated']);
                          $row['password'] = $encryption->decrypt($row['password'], 'HC_DB_P' . $row['dateCreated']);

                          $status = \HCMS\Database::runBackup($row['id'], long2ip($row['ip']), $this->settings['path'], $row['username'], $row['password'], $row['backupType'], $row['backupID']);
                          
                          $after = microtime(true);
                          
                          if($status) {
                              echo 'Success: ' . $row['backupID'] . ' - ' . $row['title']  . ' (' . $row['id'] . ') in ' . ($after - $before) . 's' . PHP_EOL;
                          } else {
                              echo 'Failure: ' . $row['backupID'] . ' - ' . $row['title']  . ' (' . $row['id'] . ')' . ($after - $before) . 's' . PHP_EOL;
                          }
                      }
                  }
              }

              echo 'Processed Manual Backups' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
