<?hh
namespace HCPublic\Ajax\Domains\Charts;

class ProcessChartDataSingleAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];

        $cache = new \HC\Cache();

        if(!$result = $cache->select('\HCPublic\Ajax\Domains\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['domainID'])) {
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
                                        `DH`.`status`,
                                        `DH`.`responseTime`,
                                        UNIX_TIMESTAMP(`DH`.`dateCreated`) as `dateCreated`
                                    FROM
                                        `domain_history` `DH`
                                    WHERE
                                        `DH`.`domainID` = ?
                                    AND 
                                        `DH`.`dateCreated` > ?
                                    GROUP BY UNIX_TIMESTAMP(`DH`.`dateCreated`) DIV ?
                                    ORDER BY `DH`.`dateCreated`;', [$POST['domainID'], $currentDate24, $divisor]);

            if($result == false) {
                $result = [];
            }
            
            $result = json_encode(['status' => 1,  'result' => $result]);
            $cache->insert('\HCPublic\Ajax\Domains\Charts\ProcessChartDataSingleAjax' . $POST['scale'] . $POST['domainID'], $result, 300);
        }

        $this->body = $result;
        return 1;
    }
}
