<?hh
namespace HCPublic\Ajax\Servers\Domain;

class ProcessAddDomainAjax extends \HC\Ajax {
    protected $settings = [
        'authentication' => true
    ];

    public function init($GET = [], $POST = []) {
        $response = [];

        // Put all errors in an array
        $response['errors'] = [];
        if(!isset($POST['data'])){
            $response['errors']['e1'] = true;
        }

        if(count($response['errors']) == 0){
            if(isset($POST['data']['serverID'])){
                $server = new \HCMS\Server(['id' => $POST['data']['serverID']]);
                if($server->checkExists()) {
                    $db = new \HC\DB();
                    if(isset($POST['data']['domainID'])) {
                        $result = $db->write('server_mapping', ['serverID' => $POST['data']['serverID'], 'domainID' => $POST['data']['domainID']]);
                        if($result) {
                            $response = ['status' => $result];
                        } else {
                            $response['errors']['e4'] = true;
                        }
                    } else {
                        $result = $db->query('SELECT 
                                                    `D`.`id`, `D`.`title`, `D`.`url`
                                                FROM
                                                    `domains` `D`
                                                        LEFT JOIN
                                                    `server_mapping` `SM` ON (`SM`.`domainID` = `D`.`id`)
                                                WHERE
                                                    `D`.`status` = 1
                                                        AND `SM`.`domainID` IS NULL
                                                        AND (`SM`.`serverID` = ? OR `SM`.`serverID` IS NULL);', [$POST['data']['serverID']]);
                        if($result) {
                            $response = ['status' => 1, 'result' => $result];
                        } else {
                            $response = ['status' => 1, 'result' => []];
                        }
                    }
                } else {
                    $response['errors']['e3'] = true;
                }
            } else {
                $response['errors']['e2'] = true;
            }
        }

        $this->body = $response;
        return 1;
    }
}
