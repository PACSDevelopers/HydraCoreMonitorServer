<?hh
namespace HCPublic\Ajax\Templates\Template;

class ProcessImportTemplateAjax extends \HC\Ajax {
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
                if(isset($POST['data']['databaseID'])) {
                    if(isset($POST['data']['schema'])) {
                        // Run import
                        $database = new \HCMS\Database(['id' => $POST['data']['databaseID']]);
                        if($database->checkExists()) {
                            $schema = $database->getSchema($POST['data']['schema']);
                            if($schema) {
                                $db->beginTransaction();
                                foreach($schema as $table => $columns) {
                                    $alias = str_replace('_', ' ', $table);
                                    $alias = ucwords($alias);
                                    
                                    $tempResult = $db->read('data_template_tables', ['id'], ['templateID' => $POST['data']['templateID'], 'name' => $table]);
                                    if($tempResult) {
                                        $tableID = $tempResult[0]['id'];
                                    } else {
                                        $db->write('data_template_tables', ['templateID' => $POST['data']['templateID'], 'name' => $table, 'alias' => $alias]);
                                        $tableID = $db->getLastID();
                                    }
                                    
                                    foreach($columns as $column) {
                                        $tempResult = $db->read('data_template_columns', ['id'], ['templateID' => $POST['data']['templateID'], 'tableID' => $tableID, 'name' => $column]);
                                        if(!$tempResult) {
                                            $alias = preg_replace(['/(?<=[^A-Z])([A-Z])/', '/(?<=[^0-9])([0-9])/'], ' $0', $column);
                                            $alias = str_replace('id', 'ID', $alias);
                                            $alias = str_replace('Id', 'ID', $alias);
                                            $alias = ucwords($alias);
                                            $alias = str_replace('#', ' #', $alias);
                                            $db->write('data_template_columns', ['templateID' => $POST['data']['templateID'], 'tableID' => $tableID, 'name' => $column, 'alias' => $alias]);
                                        }
                                    }
                                }
                                
                                $db->commit();
                                $response = ['status' => 1];
                            } else {
                                $response['errors']['e5'] = true;
                            }
                        } else {
                            $response['errors']['e3'] = true;
                        }
                    } else {
                        // Return Schemas
                        $database = new \HCMS\Database(['id' => $POST['data']['databaseID']]);
                        
                        $connection = $database->getDatabaseConnection();
                        if($connection) {
                            $result = $connection->query('SELECT `SCHEMA_NAME` FROM `INFORMATION_SCHEMA`.`SCHEMATA` WHERE `SCHEMA_NAME` NOT IN(?,?,?);', ['information_schema', 'performance_schema', 'mysql']);
                            if($result) {
                                $niceResult = [];
                                
                                foreach($result as $row) {
                                    $niceResult[] = $row['SCHEMA_NAME'];
                                }
                                
                                $response = ['status' => 1, 'result' => $niceResult];
                            } else {
                                $response = ['status' => 1, 'result' => []];
                            }
                        } else {
                            $response['errors']['e4'] = true;
                        }
                    }
                } else {
                    // Show Databases
                    $result = $db->read('databases', ['id', 'title'], ['status' => 1]);
                    if(!$result) {
                        $result = [];
                    }

                    $response = ['status' => 1, 'result' => $result];
                }
            } else {
                $response['errors']['e2'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
