<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessVaultCleanup
   * @package HC\Hooks\Cron
   */

  class ProcessVaultCleanup extends \HC\Hooks\Cron

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
          echo 'Processing Vault Cleanup' . PHP_EOL;

          $before = microtime(true) - 31536000;
          $dateTokens = explode('.', $before);
          if(!isset($dateTokens[1])) {
              $dateTokens[1] = 0;
          }

          $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
          
          $db = new \HC\DB();
          $result = $db->query('SELECT `D`.`id`, `D`.`archiveID` FROM `database_backups` `D` WHERE `D`.`status` = 3 AND `D`.`inVault` = 1 AND `D`.`dateEdited` < ?;', [$dateEdited]);
          if($result) {

              $client = \Aws\Glacier\GlacierClient::factory([
                  'key'    => $this->settings['glacier']['key'],
                  'secret' => $this->settings['glacier']['secret'],
                  'region' => $this->settings['glacier']['region']
              ]);
              
              foreach($result as $row) {
                  echo 'Deleting: ' . $row['id'] . PHP_EOL;
                  
                  $client->deleteArchive([
                      'vaultName' => 'Backups',
                      'archiveId' => $row['archiveID']
                  ]);

                  $before = microtime(true);
                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }

                  $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                  
                  $db->delete('database_backups', ['id' => $row['id']]);
              }
              
              echo 'Processed Vault Cleanup' . PHP_EOL;
              return true;
          } else {
              echo 'Processed Vault Cleanup' . PHP_EOL;
              return true;
          }
          
          return false;

      }
  }
