<?hh
namespace HCPublic\Ajax\Exports\Export;

class ScheduleAjax extends \HC\Ajax {
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
            if(isset($POST['data']['templateID'])){
                $db = new \HC\DB();
                $db->beginTransaction();
                $result = $db->write('data_exports', ['templateID' => $POST['data']['templateID'], 'databaseID' => $POST['data']['databaseID'], 'schema' => $POST['data']['schema'], 'status' => 1]);
                if($result) {
                    $exportID = $db->getLastID();
                    $commit = true;
                    foreach($POST['data']['selectedTables'] as $tableID) {
                        $result = $db->write('data_export_tables', ['exportID' => $exportID, 'tableID' => $tableID]);
                        if(!$result) {
                            $commit = false;
                            break;
                        }
                    }

                    if($commit) {
                        $response = ['status' => $db->commit()];
                    } else {
                        $response['errors']['e4'] = true;
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
