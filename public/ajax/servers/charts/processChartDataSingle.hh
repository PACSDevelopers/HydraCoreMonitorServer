<?hh
namespace HCPublic\Ajax\Servers\Charts;

class ProcessChartDataSingleAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();

        if(!$result = $cache->select('\HCPublic\Ajax\Servers\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['serverID'])) {
            $current = microtime(true);

            switch($POST['scale']) {
                case 1:
                    $current24 = $current - 86400;
                    $divisor = 600;
                    break;
                case 2:
                    $current24 = $current - 604800;
                    $divisor = 1200;
                    break;

                case 3:
                    $current24 = $current - (86400*30);
                    $divisor = 2400;
                    break;

                default:
                    $current24 = $current - 3600;
                    $divisor = 300;
                    break;
            }
            
            $dateTokens = explode('.', $current24);
            if(!isset($dateTokens[1])) {
                $dateTokens[1] = 0;
            }

            $currentDate24 = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

            $db = new \HC\DB();
            $result = $db->query('SELECT 
                                    `SH`.`status`,
                                    `SH`.`domainID`,
                                    AVG(`SH`.`cpu`) as `cpu`,
                                    AVG(`SH`.`mem`) as `mem`,
                                    AVG(`SH`.`iow`) as `iow`,
                                    AVG(`SH`.`ds`) as `ds`,
                                    SUM(`SH`.`net`) as `net`,
                                    SUM(`SH`.`rpm`) as `rpm`,
                                    SUM(`SH`.`tps`) as `tps`,
                                    SUM(`SH`.`qpm`) as `qpm`,
                                    AVG(`SH`.`avgTimeCpuBound`) `avgTimeCpuBound`,
                                    AVG(`SH`.`avgRespTime`) as `avgRespTime`,
                                    AVG(`SH`.`responseTime`) as `responseTime`,
                                    UNIX_TIMESTAMP(`SH`.`dateCreated`) as `dateCreated`
                                FROM
                                    `server_history` `SH`
                                WHERE
                                    `SH`.`serverID` = ?
                                AND 
                                    `SH`.`dateCreated` > ?
                                GROUP BY UNIX_TIMESTAMP(`SH`.`dateCreated`) DIV ?
                                ORDER BY `SH`.`dateCreated`;', [$POST['serverID'], $currentDate24, $divisor]);
            
            if($result == false) {
                $result = [];
            }
            
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Servers\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['serverID'], $result, 300);
        }

        $this->body = $result;
        return 1;
    }
}
