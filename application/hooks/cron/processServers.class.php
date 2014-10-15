<?hh


  namespace HCMS\Hooks\Cron;
  
  /**
   * Class ProcessServers
   * @package HC\Hooks\Cron
   */

  class ProcessServers extends \HC\Hooks\Cron

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
          if(is_array($settings)) {
              $this->settings = $settings;
          }
      }



      /**
       * @return bool
       */

      public function run()

      {
          echo 'Processing Servers' . PHP_EOL;
          
          $db = new \HC\DB();
          $result = $db->read([
              'servers' => 'S',
              'J.SM.server_mapping' => [
                  'SM.serverID' => 'S.id'
              ],
              'J.D.domains' => [
                  'D.id' => 'SM.domainID'
              ]
          ], ['D.id' => 'domainID', 'S.id' => 'serverID', 'D.title' => 'domainTitle', 'S.title' => 'serverTitle', 'D.url', 'S.ip'], ['S.status' => 1,'D.status' => 1]);
          
          
          if($result) {
              $before = microtime(true);
              $dateTokens = explode('.', $before);
              if(!isset($dateTokens[1])) {
                  $dateTokens[1] = 0;
              }

              $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
              
              $overview = ['up' => 0, 'dateCreated' => $dateCreated, 'responseTime' => []];
              
              $db->beginTransaction();

              foreach($result as $row) {
                  $extraData = [
                      'redirect_count' => 0,
                  ];

                  $before = microtime(true);
                  $isValidConnection = \HCMS\Server::checkHTTP(long2ip($row['ip']), $row['url'], true, $extraData);
                  $after = microtime(true) - $before;

                  $dateTokens = explode('.', $before);
                  if(!isset($dateTokens[1])) {
                      $dateTokens[1] = 0;
                  }
                  $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                  
                  if($isValidConnection === 200) {
                      $overview['up']++;
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Passed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                  } else {
                      echo $row['serverTitle'] . ' (' . $row['serverID'] . ') - ' . $row['domainTitle'] . ' (' .  $row['domainID'] . '): ' . 'Failed in ' . $after . 'ms ' . $dateCreated . PHP_EOL;
                  }

                  $overview['responseTime'][] = $after;
                  
                  $db->write('server_history', ['domainID' => $row['domainID'], 'serverID' => $row['serverID'], 'status' => $isValidConnection, 'responseTime' => $after, 'dateCreated' => $dateCreated, 'redirects' => $extraData['redirect_count']]);
              }

              $overview['responseTime'] = array_sum($overview['responseTime']) / count($overview['responseTime']);

              $overview['percent'] = ($overview['up'] / count($result)) * 100;
                
              unset($overview['up']);
              $db->write('server_history_overview', $overview);
              
              $db->commit();

              echo 'Processed Servers' . PHP_EOL;

              return true;
          } else {
              return true;
          }

          return false;

      }

  }
