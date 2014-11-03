<?hh

namespace HCPublic\Databases;

class DatabasePage extends \HC\Page {
	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Databases - ',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
									'forms' => true,
									'databaseForm' => true,
                                    'databaseTable' => true,
                                    'databaseCharts' => true,
							]
					],
                    'body' => [
                        'headerButtonsRight' => [],
                    ],
					'footer' => true
			],
			'forms' => true,
			'authentication' => true
	];

	public function init($GET = [], $POST = []) {
		if (!isset($GET['id'])) {
			return 404;
		}

		$database = new \HCMS\Database(['id' => $GET['id']]);
        
        if(!$database->checkExists()) {
            return 404;
        }
        
		$this->settings['views']['header']['pageName'] .= $database->title;
		$db = new \HC\DB();

        if($_SESSION['user']->hasPermission('Backup')) {
            $this->settings['views']['body']['headerButtonsRight'][] = [<button class="btn btn-primary" onclick="backupDatabase();">Backup Database</button>];
        }

        if($_SESSION['user']->hasPermission('Delete')) {
            $this->settings['views']['body']['headerButtonsRight'][] = [<button class="btn btn-primary" onclick="deleteDatabase();">Delete Database</button>];
        }

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
        $password = str_repeat('*', strlen($database->password));
        
        $this->body = <x:frag>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                        <div class="row col-lg-8 col-md-12 col-sm-12">
                            <h1>Database Details</h1>
                            <div class="row">
                                    <form action="" class="form-horizontal" role="form"> 
                                            <input type="hidden" name="databaseID" id="databaseID" value={$database->id} />
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseTitle">Title</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control" placeholder="Title"
                                                                        data-orgval={$database->title}
                                                                        id="databaseTitle" required="required" maxlength="50" value={$database->title} />
                                                    </div>
                                            </div>
    
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseIP">IP</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control" placeholder="IP" id="databaseIP"
                                                                        data-orgval={long2ip($database->ip)}
                                                                        required="required" value={long2ip($database->ip)} />
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseBackupType">Backup Type</label>
    
                                                    <div class="col-sm-10">
                                                            <select disabled={$isDisabled} class="form-control" id="databaseBackupType">
                                                                <option value="0" selected={$database->backupType == 0}>None</option>
                                                                <option value="1" selected={$database->backupType == 1}>MySQLDump (direct)</option>
                                                                <option value="2" selected={$database->backupType == 2}>MySQLDump (client)</option>
                                                                <option value="3" selected={$database->backupType == 3}>InnoBackupEx (client)</option>
                                                            </select>
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseBackupInterval">Backup Interval <small>(hours)</small></label>
    
                                                    <div class="col-sm-10">
                                                            <input type="number" disabled={$isDisabled} class="form-control" placeholder="Backup Interval" id="databaseBackupInterval"
                                                                        data-orgval={$database->backupInterval}
                                                                        required="required" value={$database->backupInterval} min="0" />
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseUsername">Username</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control input-force-lowercase" placeholder="Username" id="databaseUsername"
                                                                        data-orgval={$database->username}
                                                                        required="required" value={$database->username} />
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databasePassword">Password</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="password" disabled={$isDisabled} class="form-control" placeholder={$password} id="databasePassword"
                                                                        data-orgval=""
                                                                        required="required" value="" />
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseStatus">Status</label>
    
                                                    <div class="col-sm-10">
                                                            <span class="databaseStatusIcon glyphicons circle_question_mark pull-right" data-id={$database->id}></span>
                                                    </div>
                                            </div>
    
                                            <div class="form-group">
                                                    <div id="alertBox"></div>
                                                    <div class="col-sm-2"></div>
                                                    <div class="col-sm-10 text-right">
                                                        <button type="button" disabled={$isDisabled} class="btn btn-primary" onclick="updateForm();">Update Database</button>
                                                    </div>
                                            </div>
                                    </form>
                            </div>
                            <div class="row">
                                <select class="form-control" id="timeScale">
                                    <option value="0">Hour</option>
                                    <option value="1">Day</option>
                                    <option value="2">Week</option>
                                    <option value="3">Month</option>
                                </select>
                            </div>
                            <div class="row">
                                <div class="col-lg-6">
                                    <div id="historyAvailability" class="chart forceGPU noselect">
                                        <div class="spinner">
                                          <div class="rect1"></div>
                                          <div class="rect2"></div>
                                          <div class="rect3"></div>
                                          <div class="rect4"></div>
                                          <div class="rect5"></div>
                                        </div>
                                    </div>
                                </div>
    
                                <div class="col-lg-6">
                                    <div id="historyResponseTime" class="chart forceGPU noselect">
                                        <div class="spinner">
                                          <div class="rect1"></div>
                                          <div class="rect2"></div>
                                          <div class="rect3"></div>
                                          <div class="rect4"></div>
                                          <div class="rect5"></div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
