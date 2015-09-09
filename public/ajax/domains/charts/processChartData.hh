<?hh
namespace HCPublic\Ajax\Domains\Charts;

class ProcessChartDataAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];
        $cache = new \HC\Cache();

        if(!$result = $cache->select('\HCPublic\Ajax\Domains\Charts\ProcessChartDataAjax' . $POST['scale'])) {
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
                                        (COUNT(`DH`.`id`) - COUNT(`DHOFF`.`id`)) / COUNT(`DH`.`id`) * 100 as `percent`,
                                        AVG(`DHON`.`responseTime`) as `responseTime`,
                                        UNIX_TIMESTAMP(`DH`.`dateCreated`) as `dateCreated`
                                    FROM
                                        `domain_history` `DH`
                                            LEFT JOIN
                                        `domain_history` `DHON` ON (`DHON`.`id` = `DH`.`id`
                                            AND `DHON`.`status` = 200)
                                            LEFT JOIN
                                        `domain_history` `DHOFF` ON (`DHOFF`.`id` = `DH`.`id`
                                            AND `DHOFF`.`status` != 200)
                                    WHERE
                                        `DH`.`dateCreated` > ?
                                    GROUP BY UNIX_TIMESTAMP(`DH`.`dateCreated`) DIV ?
                                    ORDER BY `DH`.`dateCreated`;', [$currentDate24, $divisor]);

            if($result == false) {
                $result = [];
            }
            
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Domains\Charts\ProcessChartDataAjax' . $POST['scale'], $result, 300);
        }

        $this->body = $result;
        return 1;
    }
}
