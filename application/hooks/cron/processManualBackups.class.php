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
          echo 'Processing Manual Backups' . PHP_EOL;
          $encryption = new \HC\Encryption();
          $db = new \HC\DB();
          
          // Check already running backups
          $result = $db->read('database_backups', ['id'], ['status' => 2]);
          if($result) {
              if(count($result) >= 2) {
                  echo 'Processed Manual Backups (skipped, already running >= 2)' . PHP_EOL;
                  return true;
              }
          }
          
          $result = $db->query('SELECT `DB`.`id` as `backupID`, `DB`.`isAuto`, `DB`.`creatorID`, `D`.`id`, `D`.`title`, `D`.`intIP`, `D`.`extIP`, `D`.`username`, `D`.`password`, `D`.`backupType`, `D`.`dateCreated` FROM `database_backups` `DB` LEFT JOIN `databases` `D` ON (`D`.`id` = `DB`.`databaseID`) WHERE `DB`.`status` = 1;');
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
                      
                      $status = $db->update('database_backups', ['id' => $row['backupID'], 'status' => 1], ['status' => 2, 'dateEdited' => $dateEdited]);
                      if($status) {
                          $row['username'] = $encryption->decrypt($row['username'], 'HC_DB_U' . $row['dateCreated']);
                          $row['password'] = $encryption->decrypt($row['password'], 'HC_DB_P' . $row['dateCreated']);

                          $ipKey = 'extIP';
                          $isValidConnection = \HCMS\Database::testMySQLPort(long2ip($row['intIP']));
                          if($isValidConnection) {
                              $ipKey = 'intIP';
                          }
                          
                          try {
                              $status = \HCMS\Database::runBackup($row['id'], long2ip($row[$ipKey]), $this->settings['archive'], $row['username'], $row['password'], $row['backupType'], $row['backupID']);
                          } catch (\Exception $e) {
                              $status = false;
                          }
                          
                          
                          $after = microtime(true);
                          
                          if($status) {
                              echo 'Success: ' . $row['backupID'] . ' - ' . $row['title']  . ' (' . $row['id'] . ') in ' . ($after - $before) . 's' . PHP_EOL;
                              if($row['isAuto'] == 0) {
                                  $email = new \HC\Email();
                                  $user = $db->read('users', ['email'], ['id' => $row['creatorID']]);
                                  $email->send($user[0]['email'], 'Backup: ' . $row['title']  . ' (' . $row['id'] . ')', 'Your backup of ' . $row['title']  . ' (' . $row['id'] . ')' . ' is complete.');
                              }
                          } else {
                              
                              echo 'Failure: ' . $row['backupID'] . ' - ' . $row['title']  . ' (' . $row['id'] . ')' . ($after - $before) . 's' . PHP_EOL;
                              $email = new \HC\Email();
                              if($row['isAuto'] == 0) {
                                  $user = $db->read('users', ['email'], ['id' => $row['creatorID']]);
                                  $email->send($user[0]['email'], 'Backup: ' . $row['title']  . ' (' . $row['id'] . ')', 'Your backup of ' . $row['title']  . ' (' . $row['id'] . ')' . ' failed.');
                              } else {
                                  $db->update('databases', ['id' => $row['id']], ['lastBackUp' => 0]);
                                  $users = $db->read('users', ['email'], ['notify' => 1]);
                                  if($users) {
                                      foreach($users as $user) {
                                          $email->send($user['email'], 'Backup: ' . $row['title']  . ' (' . $row['id'] . ')', 'Your backup of ' . $row['title']  . ' (' . $row['id'] . ')' . ' failed.');
                                      }
                                  }
                              }
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
