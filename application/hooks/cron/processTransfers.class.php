<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessTransfers
   * @package HC\Hooks\Cron
   */

  class ProcessTransfers extends \HC\Hooks\Cron

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
          echo 'Processing Transfers' . PHP_EOL;
          
          
          $db = new \HC\DB();
          $result = $db->read([
              'database_transfers' => 'DT',
              'J.DB.database_backups' => [
                  'DB.id' => 'DT.backupID'
              ],
              'J.D.databases' => [
                  'D.id' => 'DT.database2ID'
              ]
          ], ['DT.id', 'DT.creatorID', 'DT.backupID', 'DB.isLocal', 'D.title', 'D.id' => 'databaseID', 'D.ip', 'D.username', 'D.password', 'D.dateCreated'], ['DT.status' => 1]);
          
          if($result) {
              $encryption = new \HC\Encryption();
              
              foreach($result as $row) {
                  $tempRow = $db->read('database_transfers', ['id'], ['id' => $row['id'], 'status' => 1]);
                  if($tempRow) {
                      $db->update('database_transfers', ['id' => $row['id']], ['status' => 2]);
                      if($row['isLocal'] == 1) {
                          $file = $this->settings['archive'] . '/' . $row['backupID'] . '.tar.xz';
                          if(is_file($file)) {
                              $row['username'] = $encryption->decrypt($row['username'], 'HC_DB_U' . $row['dateCreated']);
                              $row['password'] = $encryption->decrypt($row['password'], 'HC_DB_P' . $row['dateCreated']);

                              $transfer = \HCMS\Database::transferBackup($row['id'], $this->settings['archive'], $row['backupID'], long2ip($row['ip']), $row['username'], $row['password']);
                              if($transfer) {
                                  echo 'Success: ' . $row['id'] . PHP_EOL;
                                  $status = $db->update('database_transfers', ['id' => $row['id']], ['status' => 3]);
                                  if(!$status) {
                                      throw new \Exception('Unable to write to database');
                                  }
                                  
                                  $email = new \HC\Email();
                                  $user = $db->read('users', ['email'], ['id' => $row['creatorID']]);
                                  $email->send($user[0]['email'], 'Transfer: ' . $row['id'], 'Your transfer of backup ' . $row['backupID'] . ' to database ' . $row['title'] . ' (' . $row['databaseID'] . ') is complete.');
                                  
                              } else {
                                  echo 'Failure: ' . $row['id'] . PHP_EOL;
                                  $status = $db->update('database_transfers', ['id' => $row['id']], ['status' => 4]);
                                  if(!$status) {
                                    throw new \Exception('Unable to write to database');
                                  }
                                  
                                  $email = new \HC\Email();
                                  $user = $db->read('users', ['email'], ['id' => $row['creatorID']]);
                                  $email->send($user[0]['email'], 'Transfer: ' . $row['id'], 'Your transfer of backup ' . $row['backupID'] . ' to database ' . $row['title'] . ' (' . $row['databaseID'] . ') failed.');
                              }
                          } else {
                              $db->update('database_backups',   ['id' => $row['backupID']], ['isLocal' => 0]);
                              $db->update('database_transfers', ['id' => $row['id']], ['status' => 4]);
                          }
                      } else {
                          $db->update('database_transfers', ['id' => $row['id']], ['status' => 4]);
                      }
                  }
              }
              echo 'Processed Transfers' . PHP_EOL;
              return true;
          } else {
              echo 'Processed Transfers' . PHP_EOL;
              return true;
          }
          
          return false;

      }
  }
