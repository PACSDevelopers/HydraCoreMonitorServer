<?hh
namespace HCPublic\Ajax\Servers\Charts;

class ProcessServersUpAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];

        $db = new \HC\DB();
        $result = $db->query('SELECT
                                    ((COUNT(`S`.`id`) - SUM(`S`.`hasIssue`)) / COUNT(`S`.`id`) * 100) AS `percent`
                                FROM
                                    `servers` `S`
                                WHERE
                                    `S`.`status`= 1;');

        if($result == false) {
            $result = [];
        }

        $result = json_encode(['status' => 1,  'result' => $result]);

        $this->body = $result;
        return 1;
    }
}
