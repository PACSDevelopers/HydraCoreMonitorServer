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
          echo 'Processing Backups' . PHP_EOL;
          $db = new \HC\DB();
          $result = $db->query('SELECT `D`.`id`, `D`.`ip`, `D`.`backupType` FROM `databases` `D` WHERE `D`.`status` = 1 AND `D`.`backupType` != 0 AND `D`.`backupInterval` > 0');
          if($result) {
              foreach($result as $row) {
                  var_dump($row);
                  $result = \HCMS\Database::runBackup($row['id'], $row['ip'], $this->settings['path'], $row['backupType']);
                  var_dump($result);
              }

              \HCMS\Database::compressBackupsFully($this->settings['path']);

              echo 'Processed Backups' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
