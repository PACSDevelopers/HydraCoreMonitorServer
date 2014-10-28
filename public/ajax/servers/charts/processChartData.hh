<?hh
namespace HCPublic\Ajax\Servers\Charts;

class ProcessChartDataAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();

        if(!$result = $cache->select('\HCPublic\Ajax\Servers\Charts\ProcessDayChartAjax')) {
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
            $result = $db->query('SELECT `SHO`.`percent`, `SHO`.`responseTime`, `SHO`.`cpu`, `SHO`.`mem`, `SHO`.`iow`, `SHO`.`ds`, `SHO`.`ds`, `SHO`.`net`, `SHO`.`rpm`, `SHO`.`tps`, `SHO`.`qpm`, `SHO`.`avgTimeCpuBound`, `SHO`.`avgRespTime`, `SHO`.`dateCreated` as `dateCreated` FROM `server_history_overview` `SHO` WHERE `SHO`.`dateCreated` < ? AND `SHO`.`dateCreated` > ?;', [$currentDate, $currentDate24]);
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Servers\Charts\ProcessDayChartAjax', $result, 60);
        }

        $this->body = $result;
        return 1;
    }
}
