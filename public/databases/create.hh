<?hh
namespace HCPublic\Databases;

class CreatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Databases - Create Database',
							'scss' => [
									'main' => true
							],
							'js' => [
									'main' => true,
									'bootstrap-functions' => true,
									'forms'      => true,
									'databaseForm' => true
							]
					],
					'body' => true,
					'footer' => true
			],
			'forms' => true,
			'authentication' => true
	];

	public function init($GET = [], $POST = []) {
		$db = new \HC\DB();
        
		$this->body = <div class="container">
                        <div class="row">
                            <div class="page-header">
                                <h1>Create Database</h1>
                            </div>
    
                            <form action="" class="form-horizontal" role="form">
            
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databaseTitle">Title</label>
                                        <div class="col-sm-10">
                                                <input type="text" class="form-control" placeholder="Title" id="databaseTitle" required="required" maxlength="50" />
                                        </div>
                                </div>

                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databaseIP">IP</label>
                                        <div class="col-sm-10">
                                                <input type="text" class="form-control" placeholder="IP" id="databaseIP" required="required" />
                                        </div>
                                </div>
        
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databaseUsername">Username</label>

                                        <div class="col-sm-10">
                                                <input type="text" class="form-control input-force-lowercase" placeholder="Username" id="databaseUsername"
                                                            required="required" />
                                        </div>
                                </div>

                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databasePassword">Password</label>

                                        <div class="col-sm-10">
                                                <input type="password" class="form-control" placeholder="Password" id="databasePassword"
                                                            required="required" />
                                        </div>
                                </div>
                                
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databaseBackupType">Backup Type</label>

                                        <div class="col-sm-10">
                                                <select class="form-control" id="databaseBackupType" required="required">
                                                    <option value="0">None</option>
                                                    <option value="1">MySQLDump (direct)</option>
                                                    <option value="2">MySQLDump (client)</option>
                                                    <option value="3">InnoBackupEx (client)</option>
                                                </select>
                                        </div>
                                </div>
        
                                <div class="form-group">
                                        <label class="col-sm-2 control-label" for="databaseBackupInterval">Backup Interval <small>(hours)</small></label>

                                        <div class="col-sm-10">
                                                <input type="number" class="form-control" placeholder="Backup Interval" id="databaseBackupInterval"
                                                            required="required" min="0" value="0" />
                                        </div>
                                </div>
        
                                <div class="form-group">
                                        <div id="alertBox"></div>
                                        <button type="button" class="btn btn-default pull-right" onclick="submitForm();">Create Database</button>
                                </div>

                            </form>
                        </div>
                    </div>;
		return 1;
	}
}
