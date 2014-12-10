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
          $result = $db->read('databases', ['id', 'title', 'extIP', 'hasIssue'], ['status' => 1]);
          if($result) {
              $before = microtime(true);
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              foreach($result as $row) {

                  $database = new \HCMS\Database(['id' => $row['id']]);

                  $isValidConnection = false;
                  $isActive = false;
                  $exception = null;
                  $after = 0;
                  $before = microtime(true);
                  $time = $before;

                  if(ENVIRONMENT === 'PRODUCTION') {
                      $connection = $database->getDatabaseConnection('mysql', 1, $time, $exception);
                  } else {
                      $connection = $database->getDatabaseConnection('mysql', 2, $time, $exception);
                  }

                  if($connection) {
                      if($connection->isActive()) {
                          $isValidConnection = true;
                          $connection->disconnect();
                          $after = $time - $before;
                      }
                  }

                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                  if($isValidConnection) {
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Passed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                      // Resolve any auto issues
                      $result = $db->query('SELECT `I`.`id` FROM `issues` `I` WHERE `I`.`databaseID` = ? AND `I`.`auto` = ? AND `I`.`status` IN (1,2);', [$row['id'], 1]);
                      if($result) {
                          $db->beginTransaction();
                          foreach($result as $issue) {
                              $db->update('issues', ['id' => $issue['id']], ['status' => 3, 'dateClosed' => time()]);
                          }
                          $db->update('databases', ['id' => $row['id']], ['hasIssue' => 0]);
                          $db->commit();
                      } else if($row['hasIssue']) {
                          $db->update('databases', ['id' => $row['id']], ['hasIssue' => 0]);
                      }
                  } else {
                      $data = [
                              'Code'                    => $isValidConnection,
                              'Date'                    => $dateCreated,
                              'Database Title'          => $row['title'],
                              'Database ID'             => $row['id'],
                              'Time Elapsed'            => $after,
                              'IP Address'              => <a href={'http://' . long2ip($row['extIP'])}>{long2ip($row['extIP'])}</a>,
                              'Date'                    => $dateCreated,
                      ];
                      
                      \HCMS\Database::alertDown($data);

                      $errorID = null;
                      if($exception !== null) {
                          $error = [
                              'status' => $isValidConnection,
                              'errorCode' => $exception->getCode(),
                              'errorMessage' => $exception->getMessage(),
                          ];

                          $error['hash'] = crc32(serialize($error));

                          $errors = $db->read('errors', ['id'], ['hash' => $error['hash']]);
                          if($errors) {
                              $errorID = $errors[0]['id'];
                          } else {
                              $error['dateCreated'] = time();
                              $error = \HCMS\Error::create($error);
                              $errorID = $error->id;
                          }

                      }

                      // Check if issue exists for this error
                      $result = $db->query('SELECT `I`.`id` FROM `issues` `I` WHERE `I`.`databaseID` = ? AND `I`.`errorID` = ? AND `I`.`auto` = ? AND `I`.`status` IN (1,2);', [$row['id'], $errorID, 1]);
                      if($result) {
                          // Confirm it
                          $db->update('issues', ['id' => $result[0]['id']], ['dateLastConfirmed' => time()]);
                      } else {
                          // Create it
                          $time = time();
                          $issue = \HCMS\Issue::create([
                              'databaseID'   => $row['id'],
                              'errorID'      => $errorID,
                              'status'       => 1,
                              'dateCreated'  => $time,
                              'dateLastConfirmed' => $time,
                              'auto'         => 1
                          ]);

                          $db->update('databases', ['id' => $row['id']], ['hasIssue' => 1]);
                      }
                      echo $row['title'] . ' (' .  $row['id'] . '): ' . 'Failed in ' . $after . 'ms on ' . $dateCreated . PHP_EOL;
                  }
                  
                  $db->write('database_history', ['status' => $isValidConnection, 'databaseID' => $row['id'], 'responseTime' => $after, 'dateCreated' => $dateCreated]);
              }


              $before = (microtime(true) - (86400*30));
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

              $db->query('DELETE FROM `database_history` WHERE `dateCreated` < ?;', [$dateCreated]);

              echo 'Processed Databases' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
