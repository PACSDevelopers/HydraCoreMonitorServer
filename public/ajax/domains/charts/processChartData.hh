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

            $dateTokens = explode('.', $current24);
            if(!isset($dateTokens[1])) {
                $dateTokens[1] = 0;
            }

            $currentDate24 = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

            $db = new \HC\DB();
            $result = $db->query('SELECT `DHO`.`percent`, `DHO`.`responseTime`, UNIX_TIMESTAMP(`DHO`.`dateCreated`) as `dateCreated` FROM `domain_history_overview` `DHO` WHERE `DHO`.`dateCreated` > ?;', [$currentDate24]);

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
