<?hh
namespace HCPublic\Ajax\Servers\Charts;

class ProcessChartDataSingleAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();

        if(!$result = $cache->select('\HCPublic\Ajax\Servers\Charts\ProcessDayChartSingleAjax' . $POST['serverID'])) {
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
            $result = $db->query('SELECT `SH`.`status`, `SH`.`domainID`, `SH`.`responseTime`, `SH`.`cpu`, `SH`.`mem`, `SH`.`iow`, `SH`.`ds`, `SH`.`ds`, `SH`.`net`, `SH`.`rpm`, `SH`.`tps`, `SH`.`qpm`, `SH`.`avgTimeCpuBound`, `SH`.`avgRespTime`, UNIX_TIMESTAMP(`SH`.`dateCreated`) as `dateCreated` FROM `server_history` `SH` WHERE `SH`.`serverID` = ? AND `SH`.`dateCreated` < ? AND `SH`.`dateCreated` > ?;', [$POST['serverID'], $currentDate, $currentDate24]);
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Servers\Charts\ProcessDayChartSingleAjax' . $POST['serverID'], $result, 60);
        }

        $this->body = $result;
        return 1;
    }
}
