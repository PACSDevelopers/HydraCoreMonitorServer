<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessExports
   * @package HC\Hooks\Cron
   */

  class ProcessExports extends \HC\Hooks\Cron

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
          if(isset($globalSettings['exports'])) {
              $this->settings = $this->parseOptions($this->settings, $globalSettings['exports']);
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
          echo 'Processing Exports' . PHP_EOL;
          $db = new \HC\DB();
          $result = $db->query('SELECT `DE`.`id`, `DE`.`databaseID`, `DE`.`templateID`, `DE`.`schema` FROM `data_exports` `DE` WHERE `DE`.`status` = 1;');
          if($result) {
              foreach($result as $row) {
                  echo 'Processing: ' . $row['id'] . PHP_EOL;
                  $db->update('data_exports', ['id' => $row['id']], ['status' => 2]);
                  $export = new \HCMS\Export(['id' => $row['id']]);
                  try {
                      $result = $export->run($row['databaseID'], $row['schema'], $row['templateID']);
                  } catch (\Exception $e) {
                      $result = false;
                      $db->update('data_exports', ['id' => $row['id']], ['status' => 4]);
                      var_dump($e);
                  }

                  var_dump($result);
                  if($result) {
                      echo 'Export Success: ' . $row['id'] . PHP_EOL;
                  } else {
                      echo 'Export Failure: ' . $row['id'] . PHP_EOL;
                  }
              }

              echo 'Processed Exports' . PHP_EOL;
              
              return true;
          } else {
              return true;
          }
          
          return false;

      }
  }
