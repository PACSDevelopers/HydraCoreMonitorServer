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

        $backupsTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover']);
        
        $backups = $db->read('database_backups', ['id', 'progress', 'isLocal', 'status', 'dateStarted']);
        if($backups) {
            $backupsTable->openHeader();
            $backupsTable->column(['value' => 'ID']);
            $backupsTable->column(['value' => 'Status']);
            $backupsTable->column(['value' => 'Available']);
            $backupsTable->column(['value' => 'Date']);
            $backupsTable->column(['value' => 'Progress']);
            $backupsTable->closeHeader();
            
            $backupsTable->openBody();

            $backups = array_reverse($backups);
            $statusArray = [
                1 => 'Scheduled',
                2 => 'Started',
                3 => 'Complete',
                4 => 'Failed'
            ];
            
            foreach($backups as $backup) {
                $backupsTable->openRow();
                $backupsTable->column(['value' => <span>{$backup['id']}</span>]);
                $backupsTable->column(['value' => <span>{$statusArray[$backup['status']]}</span>]);
                
                switch($backup['status']) {
                    case 2:
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        $backupsTable->column(['style' => 'width: 70%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 3:
                        if($backup['isLocal']) {
                            $backupsTable->column(['value' => <span class="glyphicons circle_ok" style="color: #53A93F;"></span>]);
                        } else {
                            $backupsTable->column(['value' => <span class="glyphicons circle_arrow_down" style="color: #158cba;"></span>]);
                        }

                        $backupsTable->column(['value' => <span><a href={'/downloads/backups/' . $backup['id']}>{$backup['dateStarted']}</a></span>]);
                        
                        $backupsTable->column(['style' => 'width: 70%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 4:
                        $backupsTable->column(['value' => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        $backupsTable->column(['style' => 'width: 70%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    default:
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        $backupsTable->column(['style' => 'width: 70%', 'value' => <div class="progress">
                                                      <div class="progress-bar" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                }
                
                $backupsTable->closeRow();
            }
            
            $backupsTable->closeBody();
        }

        
        
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
                            <div class="row">
                                <div class="table-responsive">
                                    {$backupsTable}
                                </div>
                            </div>
                        </div>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
