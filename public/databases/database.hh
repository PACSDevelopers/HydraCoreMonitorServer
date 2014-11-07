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

        $statusArray = [
            1 => 'Scheduled',
            2 => 'In Progress',
            3 => 'Complete',
            4 => 'Failed'
        ];
        
        $backupsTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover']);
        
        $backups = $db->read('database_backups', ['id', 'progress', 'isLocal', 'inVault', 'isAuto', 'hasJob', 'status', 'dateStarted'], ['databaseID' => $GET['id']]);
        if($backups) {
            $backupsTable->openHeader();
            $backupsTable->column(['value' => 'ID']);
            $backupsTable->column(['value' => 'Status']);
            $backupsTable->column(['value' => 'Available']);
            $backupsTable->column(['value' => 'In Vault']);
            $backupsTable->column(['value' => 'Date']);
            $backupsTable->column(['value' => 'Type']);
            $backupsTable->column(['value' => 'Progress']);
            $backupsTable->column(['value' => 'Action']);
            $backupsTable->closeHeader();
            
            $backupsTable->openBody();

            $backups = array_reverse($backups);
            
            foreach($backups as $backup) {
                $backupsTable->openRow();
                $backupsTable->column(['value' => <span>{$backup['id']}</span>]);
                $backupsTable->column(['value' => <span>{$statusArray[$backup['status']]}</span>]);
                
                switch($backup['status']) {
                    case 2:
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        if($backup['isAuto']) {
                            $backupsTable->column(['value' => <span>Auto</span>]);
                        } else {
                            $backupsTable->column(['value' => <span>Manual</span>]);
                        }
                        $backupsTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 3:
                        if($backup['isLocal']) {
                            $backupsTable->column(['value' => <span class="glyphicons circle_ok" style="color: #53A93F;"></span>]);
                        } else {
                            if($backup['inVault']) {
                                if($backup['hasJob'] == 0) {
                                    $backupsTable->column(['value' => <span class="glyphicons circle_arrow_down" style="color: #158cba;"></span>]);
                                } else {
                                    $backupsTable->column(['value' => <span class="glyphicons circle_info"></span>]);
                                }
                            } else {
                                $backupsTable->column(['value' => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>]);
                            }
                        }

                        if($backup['inVault']) {
                            $backupsTable->column(['value' => <span class="glyphicons circle_ok" style="color: #53A93F;"></span>]);
                        } else {
                            if($backup['isLocal']) {
                                $backupsTable->column(['value' => <span class="glyphicons circle_arrow_top" style="color: #158cba;"></span>]);
                            } else {
                                $backupsTable->column(['value' => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>]);
                            }
                        }

                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        
                        if($backup['isAuto']) {
                            $backupsTable->column(['value' => <span>Auto</span>]);
                        } else {
                            $backupsTable->column(['value' => <span>Manual</span>]);
                        }
                        
                        $backupsTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 4:
                        $backupsTable->column(['value' => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>]);
                        $backupsTable->column(['value' => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        if($backup['isAuto']) {
                            $backupsTable->column(['value' => <span>Auto</span>]);
                        } else {
                            $backupsTable->column(['value' => <span>Manual</span>]);
                        }
                        $backupsTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    default:
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span class="glyphicons circle_question_mark"></span>]);
                        $backupsTable->column(['value' => <span>{$backup['dateStarted']}</span>]);
                        if($backup['isAuto']) {
                            $backupsTable->column(['value' => <span>Auto</span>]);
                        } else {
                            $backupsTable->column(['value' => <span>Manual</span>]);
                        }
                        $backupsTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                }
                
                if($backup['status'] == 3) {
                    $list = <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDown' . $backup['id']}></ul>;
                    $hasChildren = false;
                    if($backup['isLocal']) {
                        $hasChildren = true;
                        $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/backups/' . $backup['id']}>Download</a></li>);
                        $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'transferBackup(' . $backup['id'] . ');'}>Transfer</a></li>);
                        $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'deleteBackup(' . $backup['id'] . ');'}>Delete</a></li>);
                    }
                    
                    if($backup['inVault']) {
                        if($hasChildren) {
                            $list->appendChild(<li role="presentation" class="divider"></li>);
                        } else {
                            if($backup['hasJob'] == 0) {
                                $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'getArchiveFromVault(' . $backup['id'] . ');'}>Request From Vault</a></li>);
                            }
                        }
                        $hasChildren = true;
                        $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'deleteArchiveFromVault(' . $backup['id'] . ');'}>Delete From Vault</a></li>);
                    }
                    
                    if($hasChildren) {
                        $backupsTable->column(['value' => <div class="dropdown">
                                                          <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDown' . $backup['id']} data-toggle="dropdown">
                                                            <span class="caret"></span>
                                                          </button>
                                                            {$list}
                                                        </div>]);
                    } else {
                        $backupsTable->column();
                    }
                } else if($backup['status'] == 2) {
                    $backupsTable->column(['value' => <div class="dropdown">
                                                          <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDown' . $backup['id']} data-toggle="dropdown">
                                                            <span class="caret"></span>
                                                          </button>
                                                          <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDown' . $backup['id']}>
                                                            <li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'stopBackup(' . $backup['id'] . ');'}>Stop</a></li>
                                                          </ul>
                                                        </div>]);
                } else {
                    $backupsTable->column();
                }
                
                
                
                
                
                $backupsTable->closeRow();
            }
            
            $backupsTable->closeBody();
        }

        $transfersTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover']);

        $transfers = $db->read(
            ['database_transfers' => 'DT', 'J.D.databases' => [
                'D.id' => 'DT.database2ID'
            ]], ['DT.id', 'DT.database2ID', 'D.title', 'DT.backupID', 'DT.status', 'DT.progress', 'DT.dateCreated'], ['DT.database1ID' => $GET['id']]);
        if($transfers) {
            $transfersTable->openHeader();
            $transfersTable->column(['value' => 'ID']);
            $transfersTable->column(['value' => 'To']);
            $transfersTable->column(['value' => 'Backup']);
            $transfersTable->column(['value' => 'Status']);
            $transfersTable->column(['value' => 'Date Created']);
            $transfersTable->column(['value' => 'Progress']);
            $transfersTable->column(['value' => 'Action']);
            $transfersTable->closeHeader();

            $transfersTable->openBody();

            $transfers = array_reverse($transfers);

            foreach($transfers as $transfer) {
                $transfersTable->openRow();
                $transfersTable->column(['value' => <span>{$transfer['id']}</span>]);
                $transfersTable->column(['value' => <span>{$transfer['title']}</span>]);
                $transfersTable->column(['value' => <span>{$transfer['backupID']}</span>]);
                $transfersTable->column(['value' => <span>{$statusArray[$transfer['status']]}</span>]);
                $transfersTable->column(['value' => <span>{$transfer['dateCreated']}</span>]);
                
                switch($transfer['status']) {
                    case 2:
                        $transfersTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 3:
                        $transfersTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    case 4:
                        $transfersTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                    default:
                        $transfersTable->column(['style' => 'width: 50%', 'value' => <div class="progress">
                                                      <div class="progress-bar" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>]);
                        break;
                }
                
                if($transfer['status'] == 2) {
                    $transfersTable->column(['value' => <div class="dropdown">
                                                          <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDownTransfer' . $transfer['id']} data-toggle="dropdown">
                                                            <span class="caret"></span>
                                                          </button>
                                                          <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDownTransfer' . $transfer['id']}>
                                                            <li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'stopTransfer(' . $transfer['id'] . ');'}>Stop</a></li>
                                                          </ul>
                                                        </div>]);
                } else {
                    $transfersTable->column();
                }
                
                $transfersTable->closeRow();
            }

            $transfersTable->closeBody();
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
                                                    <label class="col-sm-2 control-label" for="databaseExtIP">External IP</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control" placeholder="External IP" id="databaseExtIP"
                                                                        data-orgval={long2ip($database->extIP)}
                                                                        required="required" value={long2ip($database->extIP)} />
                                                    </div>
                                            </div>
        
                                            <div class="form-group">
                                                    <label class="col-sm-2 control-label" for="databaseIntIP">Internal IP</label>
    
                                                    <div class="col-sm-10">
                                                            <input type="text" disabled={$isDisabled} class="form-control" placeholder="Internal IP" id="databaseIntIP"
                                                                        data-orgval={long2ip($database->intIP)}
                                                                        required="required" value={long2ip($database->intIP)} />
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
                                <h1>Backups</h1>
                                <div class="table-responsive">
                                    {$backupsTable}
                                </div>
                            </div>
                            <div class="row">
                                <h1>Transfers</h1>
                                <div class="table-responsive">
                                    {$transfersTable}
                                </div>
                            </div>
                        </div>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
