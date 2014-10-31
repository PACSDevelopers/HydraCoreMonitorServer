<?hh
namespace HCPublic\Ajax\Databases\Charts;

class ProcessChartDataSingleAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();
        
        if(!$result = $cache->select('\HCPublic\Ajax\Databases\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['databaseID'])) {
            $current = microtime(true);

            switch($POST['scale']) {
                case 1:
                    $current24 = $current - 86400;
                    break;
                case 2:
                    $current24 = $current - 604800;
                    break;

                case 3:
                    $current24 = $current - (86400*30);
                    break;

                default:
                    $current24 = $current - 3600;
                    break;
            }

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
            $result = $db->query('SELECT `DH`.`status`, `DH`.`responseTime`, UNIX_TIMESTAMP(`DH`.`dateCreated`) as `dateCreated` FROM `database_history` `DH` WHERE `DH`.`databaseID` = ? AND `DH`.`dateCreated` < ? AND `DH`.`dateCreated` > ?;', [$POST['databaseID'], $currentDate, $currentDate24]);

            if($result == false) {
                $result = [];
            }

            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Databases\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['databaseID'], $result, 60);
        }
        
        $this->body = $result; 
        return 1;
    }
}
