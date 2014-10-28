<?hh
namespace HCPublic\Ajax\Databases\Charts;

class ProcessChartDataSingleAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();
        
        if(!$result = $cache->select('\HCPublic\Ajax\Databases\Charts\ProcessDayChartSingleAjax' . $POST['databaseID'])) {
            $current = microtime(true);
            $current24 = $current - (86400*30);
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
            $result = $db->query('SELECT `DH`.`status`, `DH`.`responseTime`, `DH`.`dateCreated` as `dateCreated` FROM `database_history` `DH` WHERE `DH`.`databaseID` = ? AND `DH`.`dateCreated` < ? AND `DH`.`dateCreated` > ?;', [$POST['databaseID'], $currentDate, $currentDate24]);
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Databases\Charts\ProcessDayChartSingleAjax' . $POST['databaseID'], $result, 60);
        }
        
        $this->body = $result; 
        return 1;
    }
}
