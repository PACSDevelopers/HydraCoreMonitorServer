<?hh
namespace HCPublic\Ajax\Servers\Charts;

class ProcessChartDataAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        
        $cache = new \HC\Cache();
        
        if(!$result = $cache->select('\HCPublic\Ajax\Servers\Charts\ProcessChartDataAjax' . $POST['scale'])) {
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
                                    (COUNT(`SH`.`id`) - COUNT(`SHOFF`.`id`)) / COUNT(`SH`.`id`) * 100 as `percent`,
                                    AVG(`SHON`.`cpu`) as `cpu`,
                                    AVG(`SHON`.`mem`) as `mem`,
                                    AVG(`SHON`.`iow`) as `iow`,
                                    AVG(`SHON`.`ds`) as `ds`,
                                    SUM(`SHON`.`net`) as `net`,
                                    SUM(`SHON`.`rpm`) as `rpm`,
                                    SUM(`SHON`.`tps`) as `tps`,
                                    SUM(`SHON`.`qpm`) as `qpm`,
                                    AVG(`SHON`.`avgTimeCpuBound`) `avgTimeCpuBound`,
                                    AVG(`SHON`.`avgRespTime`) as `avgRespTime`,
                                    AVG(`SHON`.`responseTime`) as `responseTime`,
                                    UNIX_TIMESTAMP(`SH`.`dateCreated`) as `dateCreated`
                                FROM
                                    `server_history` `SH`
                                        LEFT JOIN
                                    `server_history` `SHON` ON (`SHON`.`id` = `SH`.`id`
                                        AND `SHON`.`status` = 200)
                                        LEFT JOIN
                                    `server_history` `SHOFF` ON (`SHOFF`.`id` = `SH`.`id`
                                        AND `SHOFF`.`status` != 200)
                                WHERE
                                    `SH`.`dateCreated` > ?
                                GROUP BY UNIX_TIMESTAMP(`SH`.`dateCreated`) DIV ?
                                ORDER BY `SH`.`dateCreated`;', [$currentDate24, $divisor]);
            
            if($result == false) {
                $result = [];
            }

            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Servers\Charts\ProcessChartDataAjax' . $POST['scale'], $result, 300);
        }

        $this->body = $result;
        return 1;
    }
}
