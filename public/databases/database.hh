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
									'databases/databaseForm' => true,
                                    'databases/databaseTable' => true,
                                    'databases/databaseCharts' => true,
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
            $this->settings['views']['body']['headerButtonsRight'][] = [<button class="btn btn-primary pull-right" onclick="backupDatabase();">Backup Database</button>];
        }

        if($_SESSION['user']->hasPermission('Delete')) {
            $this->settings['views']['body']['headerButtonsRight'][] = [<button class="btn btn-primary pull-right" onclick="deleteDatabase();">Delete Database</button>];
        }

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
        $password = str_repeat('*', strlen($database->password));

        $statusArray = [
            1 => 'Scheduled',
            2 => 'In Progress',
            3 => 'Complete',
            4 => 'Failed'
        ];
        
        $backupsHeader = <tr></tr>;
        $backupsBody = <tbody></tbody>;
        
        $backups = $db->read('database_backups', ['id', 'progress', 'isLocal', 'inVault', 'isAuto', 'hasJob', 'status', 'dateStarted', 'dateEnded'], ['databaseID' => $GET['id']]);
        if($backups) {

            $backupsHeader->appendChild(<th>ID</th>);
            $backupsHeader->appendChild(<th>Status</th>);
            $backupsHeader->appendChild(<th>Available</th>);
            $backupsHeader->appendChild(<th>In Vault</th>);
            $backupsHeader->appendChild(<th>Date Started</th>);
            $backupsHeader->appendChild(<th>Date Ended</th>);
            $backupsHeader->appendChild(<th>Type</th>);
            $backupsHeader->appendChild(<th>Progress</th>);
            $backupsHeader->appendChild(<th>Action</th>);
            

            $backups = array_reverse($backups);
            
            foreach($backups as $backup) {
                $backupsRow = <tr></tr>;
                $backupsRow->appendChild(<td><span>{$backup['id']}</span></td>);
                $backupsRow->appendChild(<td><span>{$statusArray[$backup['status']]}</span></td>);
                
                switch($backup['status']) {
                    case 2:
                        $backupsRow->appendChild(<td><span class="glyphicons circle_question_mark"></span></td>);
                        $backupsRow->appendChild(<td><span class="glyphicons circle_question_mark"></span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateStarted']}</span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateEnded']}</span></td>);
                        if($backup['isAuto']) {
                            $backupsRow->appendChild(<td><span>Auto</span></td>);
                        } else {
                            $backupsRow->appendChild(<td><span>Manual</span></td>);
                        }
                        
                        $backupsRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>
                                                </td>);
                        break;
                    case 3:
                        if($backup['isLocal']) {
                            $backupsRow->appendChild(<td><span class="glyphicons circle_ok" style="color: #53A93F;"></span></td>);
                        } else {
                            if($backup['inVault']) {
                                if($backup['hasJob'] == 0) {
                                    $backupsRow->appendChild(<td><span class="glyphicons circle_arrow_down" style="color: #158cba;"></span></td>);
                                } else {
                                    $backupsRow->appendChild(<td><span class="glyphicons circle_info"></span></td>);
                                }
                            } else {
                                $backupsRow->appendChild(<td><span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span></td>);
                            }
                        }

                        if($backup['inVault']) {
                            $backupsRow->appendChild(<td><span class="glyphicons circle_ok" style="color: #53A93F;"></span></td>);
                        } else {
                            if($backup['isLocal']) {
                                $backupsRow->appendChild(<td><span class="glyphicons circle_arrow_top" style="color: #158cba;"></span></td>);
                            } else {
                                $backupsRow->appendChild(<td><span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span></td>);
                            }
                        }

                        $backupsRow->appendChild(<td><span>{$backup['dateStarted']}</span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateEnded']}</span></td>);
                        
                        if($backup['isAuto']) {
                            $backupsRow->appendChild(<td><span>Auto</span></td>);
                        } else {
                            $backupsRow->appendChild(<td><span>Manual</span></td>);
                        }
                        
                        $backupsRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>
                                                </td>);
                        break;
                    case 4:
                        $backupsRow->appendChild(<td><span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span></td>);
                        $backupsRow->appendChild(<td><span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateStarted']}</span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateEnded']}</span></td>);
                        if($backup['isAuto']) {
                            $backupsRow->appendChild(<td><span>Auto</span></td>);
                        } else {
                            $backupsRow->appendChild(<td><span>Manual</span></td>);
                        }
                        $backupsRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>
                                                </td>);
                        break;
                    default:
                        $backupsRow->appendChild(<td><span class="glyphicons circle_question_mark"></span></td>);
                        $backupsRow->appendChild(<td><span class="glyphicons circle_question_mark"></span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateStarted']}</span></td>);
                        $backupsRow->appendChild(<td><span>{$backup['dateEnded']}</span></td>);
                        if($backup['isAuto']) {
                            $backupsRow->appendChild(<td><span>Auto</span></td>);
                        } else {
                            $backupsRow->appendChild(<td><span>Manual</span></td>);
                        }
                        $backupsRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar" role="progressbar" aria-valuenow={$backup['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $backup['progress'] . '%;'}>
                                                        <span class="sr-only">$backup['progress']</span>
                                                      </div>
                                                    </div>
                                                </td>);
                        break;
                }
                
                if($backup['status'] == 3 && $_SESSION['user']->hasPermission('Backup')) {
                    $list = <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDown' . $backup['id']}></ul>;
                    $hasChildren = false;
                    if($backup['isLocal']) {
                        $hasChildren = true;
                        if($_SESSION['user']->hasPermission('Download Backup')) {
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/backups/' . $backup['id']}>Download</a></li>);
                        }
                        
                        if($_SESSION['user']->hasPermission('Transfer Backup')) {
                            $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'transferBackup(' . $backup['id'] . ');'}>Transfer</a></li>);
                        }
                        
                        $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'deleteBackup(' . $backup['id'] . ');'}>Delete</a></li>);
                    }
                    
                    if($backup['inVault'] && ($_SESSION['user']->hasPermission('Vault Download') || $_SESSION['user']->hasPermission('Vault Delete'))) {
                        if($hasChildren) {
                            $list->appendChild(<li role="presentation" class="divider"></li>);
                        }

                        if($backup['hasJob'] == 0 && $_SESSION['user']->hasPermission('Vault Download')) {
                            $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'getArchiveFromVault(' . $backup['id'] . ');'}>Request From Vault</a></li>);
                            $hasChildren = true;
                        }
                        
                        if($_SESSION['user']->hasPermission('Vault Delete')) {
                            $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'deleteArchiveFromVault(' . $backup['id'] . ');'}>Delete From Vault</a></li>);
                            $hasChildren = true;
                        }
                    }
                    
                    if($hasChildren) {
                        $backupsRow->appendChild(<td>
                                                    <div class="dropdown">
                                                      <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDown' . $backup['id']} data-toggle="dropdown">
                                                        <span class="caret"></span>
                                                      </button>
                                                        {$list}
                                                    </div>
                                                </td>);
                    } else {
                        $backupsRow->appendChild(<td></td>);
                    }
                } else if($backup['status'] == 2 && $_SESSION['user']->hasPermission('Backup')) {
                    $backupsRow->appendChild(<td>
                                                <div class="dropdown">
                                                  <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDown' . $backup['id']} data-toggle="dropdown">
                                                    <span class="caret"></span>
                                                  </button>
                                                  <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDown' . $backup['id']}>
                                                    <li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'stopBackup(' . $backup['id'] . ');'}>Stop</a></li>
                                                  </ul>
                                                </div>
                                            </td>);
                } else {
                    $backupsRow->appendChild(<td></td>);
                }
                
                $backupsBody->appendChild($backupsRow);
            }
        }
        
        $transfersHeader = <tr></tr>;
        $transfersBody = <tbody></tbody>;
        $transfers = $db->read(
            ['database_transfers' => 'DT', 'J.D.databases' => [
                'D.id' => 'DT.database2ID'
            ]], ['DT.id', 'DT.database2ID', 'D.title', 'DT.backupID', 'DT.status', 'DT.progress', 'DT.dateCreated'], ['DT.database1ID' => $GET['id']]);
        if($transfers) {
            $transfersHeader->appendChild(<th>ID</th>);
            $transfersHeader->appendChild(<th>To</th>);
            $transfersHeader->appendChild(<th>Content</th>);
            $transfersHeader->appendChild(<th>Status</th>);
            $transfersHeader->appendChild(<th>Date Created</th>);
            $transfersHeader->appendChild(<th>Progress</th>);
            $transfersHeader->appendChild(<th>Action</th>);

            $transfers = array_reverse($transfers);

            foreach($transfers as $transfer) {
                $transfersRow = <tr></tr>;
                $transfersRow->appendChild(<td><span>{$transfer['id']}</span></td>);
                $transfersRow->appendChild(<td><span>{$transfer['title']}</span></td>);
                if($transfer['backupID']) {
                    $transfersRow->appendChild(<td><span>Backup {$transfer['backupID']}</span></td>);
                } else {
                    $transfersRow->appendChild(<td><span>Direct</span></td>);
                }
                $transfersRow->appendChild(<td><span>{$statusArray[$transfer['status']]}</span></td>);
                $transfersRow->appendChild(<td><span>{$transfer['dateCreated']}</span></td>);
                
                switch($transfer['status']) {
                    case 2:
                        $transfersRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>
                                                    </td>);
                        break;
                    case 3:
                        $transfersRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>
                                                    </td>);
                        break;
                    case 4:
                        $transfersRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>
                                                    </td>);
                        break;
                    default:
                        $transfersRow->appendChild(<td style="width: 30%;">
                                                    <div class="progress">
                                                      <div class="progress-bar" role="progressbar" aria-valuenow={$transfer['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $transfer['progress'] . '%;'}>
                                                        <span class="sr-only">$transfer['progress']</span>
                                                      </div>
                                                    </div>
                                                    </td>);
                        break;
                }
                
                if($transfer['status'] == 2) {
                    $transfersRow->appendChild(<td>
                                                    <div class="dropdown">
                                                          <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDownTransfer' . $transfer['id']} data-toggle="dropdown">
                                                            <span class="caret"></span>
                                                          </button>
                                                          <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDownTransfer' . $transfer['id']}>
                                                            <li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'stopTransfer(' . $transfer['id'] . ');'}>Stop</a></li>
                                                          </ul>
                                                        </div>
                                                    </td>);
                } else {
                    $transfersRow->appendChild(<td></td>);
                }
                
                $transfersBody->appendChild($transfersRow);
            }
        }
        
        $this->body = <x:frag>
                        <div class="container">
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
                                    <table class="table table-bordered table-striped table-hover" id="backupsTable">
                                        <thead>
                                            {$backupsHeader}
                                        </thead>
                                        {$backupsBody}
                                    </table>
                                </div>
                            </div>
                            <div class="row">
                                <h1>Transfers</h1>
                                <div class="table-responsive">
                                    <table class="table table-bordered table-striped table-hover" id="transfersTable">
                                        <thead>
                                            {$transfersHeader}
                                        </thead>
                                        {$transfersBody}
                                    </table>
                                </div>
                            </div>
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
