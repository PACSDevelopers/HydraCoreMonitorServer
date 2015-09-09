<?hh
namespace HCPublic\Ajax\Domains\Charts;

class ProcessDomainsUpAjax extends \HC\Ajax {
    protected $settings = [];

    public function init($GET = [], $POST = []) {
        $response = [];

        $db = new \HC\DB();
        $result = $db->query('SELECT
                                    ((COUNT(`D`.`id`) - SUM(`D`.`hasIssue`)) / COUNT(`D`.`id`) * 100) AS `percent`
                                FROM
                                    `domains` `D`
                                WHERE
                                    `D`.`status`= 1;');

        if($result == false) {
            $result = [];
        }

        $result = json_encode(['status' => 1,  'result' => $result]);

        $this->body = $result;
        return 1;
    }
}
