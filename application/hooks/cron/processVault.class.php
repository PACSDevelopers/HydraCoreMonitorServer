<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessVault
   * @package HC\Hooks\Cron
   */

  class ProcessVault extends \HC\Hooks\Cron

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
          echo 'Processing Vault' . PHP_EOL;
          $db = new \HC\DB();
          $result = $db->query('SELECT `D`.`id`, `D`.`hasJob`, `D`.`jobID` FROM `database_backups` `D` WHERE (`D`.`status` = 3 AND `D`.`inVault` = 0 AND `D`.`isLocal` = 1) OR (`D`.`hasJob` = 1);');
          if($result) {
              $client = \Aws\Glacier\GlacierClient::factory([
                  'key'    => $this->settings['glacier']['key'],
                  'secret' => $this->settings['glacier']['secret'],
                  'region' => $this->settings['glacier']['region']
              ]);
              
              foreach($result as $row) {
                  $rowResult = $db->query('SELECT `D`.`id` FROM `database_backups` `D` WHERE ((`D`.`status` = 3 AND `D`.`inVault` = 0 AND `D`.`isLocal` = 1) OR (`D`.`hasJob` = 1)) AND `D`.`id` = ?;', [$row['id']]);
                  if($rowResult) {
                      echo 'Processing: ' . $row['id'] . PHP_EOL;
                      if($row['hasJob'] == 0) {
                          echo 'Upload' . PHP_EOL;
                          if(is_file($this->settings['archive'] . '/' . $row['id'] . '.tar.xz')) {
                              $archiveData = fopen($this->settings['archive'] . '/' . $row['id'] . '.tar.xz', 'r');
                              
                              $partSize = 4 * 1024 * 1024;
                              $parts = \Aws\Glacier\Model\MultipartUpload\UploadPartGenerator::factory($archiveData, $partSize);

                              $archiveData = fopen($this->settings['archive'] . '/' . $row['id'] . '.tar.xz', 'r');
                              
                              $result = $client->initiateMultipartUpload([
                                  'vaultName' => 'Backups',
                                  'partSize'  => $partSize,
                              ]);
                              
                              $uploadId = $result->get('uploadId');
                              
                              foreach ($parts as $part) {

                                  fseek($archiveData, $part->getOffset());
                                  $client->uploadMultipartPart([
                                      'vaultName'     => 'Backups',
                                      'uploadId'      => $uploadId,
                                      'body'          => fread($archiveData, $part->getSize()),
                                      'range'         => $part->getFormattedRange(),
                                      'checksum'      => $part->getChecksum(),
                                      'ContentSHA256' => $part->getContentHash(),
                                  ]);
                              }

                              // Complete the upload by using data aggregated by the part generator
                              $result = $client->completeMultipartUpload(array(
                                  'vaultName'   => 'Backups',
                                  'uploadId'    => $uploadId,
                                  'archiveSize' => $parts->getArchiveSize(),
                                  'checksum'    => $parts->getRootChecksum(),
                              ));
                              
                              $archiveId = $result->get('archiveId');

                              fclose($archiveData);
                              
                              if($archiveId) {
                                  $db->update('database_backups', ['id' => $row['id']], ['inVault' => 1, 'archiveID' => $archiveId]);
                                  echo 'Success: ' . $row['id'] . PHP_EOL;
                              } else {
                                  echo 'Failed: ' . $row['id'] . PHP_EOL;
                              }
                          } else {
                              $db->update('database_backups', ['id' => $row['id']], ['isLocal' => 0]);
                              echo 'Failed: ' . $row['id'] . PHP_EOL;
                          }
                      } else {
                          echo 'Download: ' . $row['id'] . PHP_EOL;
                          $glacierResult = $client->describeJob([
                              'vaultName' => 'Backups',
                              'jobId'      => $row['jobID'],
                          ]);
                          
                          if($glacierResult) {
                              if($glacierResult['Completed']) {
                                  $handle = fopen($this->settings['archive'] . '/' . $row['id'] . '.tar.xz', 'w');
                                  
                                  $glacierResult = $client->getJobOutput([
                                      'vaultName' => 'Backups',
                                      'jobId'      => $row['jobID'],
                                      'saveAs'     => $handle,
                                      'timeout'  => 600,
                                      'connect_timeout' => 600,
                                      'curl.options' => [
                                          CURLOPT_TIMEOUT => 600,
                                          CURLOPT_CONNECTTIMEOUT => 600
                                      ],
                                      'request.options' => [
                                          'timeout'  => 600,
                                          'connect_timeout' => 600
                                      ]
                                  ]);
                                  
                                  if($glacierResult) {
                                      if($glacierResult['status'] == 200) {
                                          $before = microtime(true);
                                          $dateTokens = explode('.', $before);
                                          if(!isset($dateTokens[1])) {
                                              $dateTokens[1] = 0;
                                          }

                                          $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                                          $db->update('database_backups', ['id' => $row['id']], ['isLocal' => 1, 'hasJob' => 0, 'jobID' => '', 'dateEdited' => $dateEdited]);

                                          $users = $db->read('users', ['email'], ['notify' => 1]);
                                          if($users) {
                                              $email = new \HC\Email();
                                              foreach($users as $user) {
                                                  $email->send($user['email'], 'Backup: (' . $row['id'] . ') Now Available', 'Your backup of (' . $row['id'] . ')' . ' has been downloaded from the vault and is ready for download.');
                                              }
                                          }
                                          echo 'Success: ' . $row['id'] . PHP_EOL;
                                          
                                          continue;
                                          
                                      } else {
                                          echo 'Not Ready: ' . $row['id'] . PHP_EOL;
                                          continue;
                                      }
                                  }
                              } else {
                                  echo 'Not Ready: ' . $row['id'] . PHP_EOL;
                                  continue;
                              }
                          }

                          echo 'Failed: ' . $row['id'] . PHP_EOL;
                      }
                  }
              }
              
              

              echo 'Processed Vault' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
