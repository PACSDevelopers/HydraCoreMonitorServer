<?hh
namespace HCPublic\Ajax\Databases\Charts;

class ProcessChartDataAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();
        
        if(!$result = $cache->select('\HCPublic\Ajax\Databases\Charts\ProcessDayChartAjax')) {
            $current = microtime(true);
            $current24 = $current - 2592000;
            $dateTokens = explode('.', $current);
            if(!isset($dateTokens[1])) {
                $dateTokens[1] = 0;
            }

            $currentDate = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

            $dateTokens = explode('.', $current24);
            if(!isset($dateTokens[1])) {
                $dateTokens[1] = 0;
            }

            $currentDate24 = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

            $db = new \HC\DB();
            $result = $db->query('SELECT `DHO`.`percent`, `DHO`.`responseTime`, `DHO`.`dateCreated` as `dateCreated` FROM `database_history_overview` `DHO` WHERE `DHO`.`dateCreated` < ? AND `DHO`.`dateCreated` > ?;', [$currentDate, $currentDate24]);
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Databases\Charts\ProcessDayChartAjax', $result, 60);
        }
        
        $this->body = $result; 
        return 1;
    }
}
