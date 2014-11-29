<?hh

namespace HCPublic\Servers;

class ServerPage extends \HC\Page {
	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Servers - ',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
									'forms' => true,
									'servers/serverForm' => true,
                                    'servers/serverTable' => true,
                                    'servers/serverCharts' => true
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

		$server = new \HCMS\Server(['id' => $GET['id']]);
        
        if(!$server->checkExists()) {
            return 404;
        }
        
		$this->settings['views']['header']['pageName'] .= $server->title;
		$db = new \HC\DB();

        if($_SESSION['user']->hasPermission('Delete')) {
            $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="deleteServer();">Delete Server</button>;
        }

        if($_SESSION['user']->hasPermission('Restart Server')) {
            $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="restartServer();">Restart Server</button>;
        }

        if($_SESSION['user']->hasPermission('Reboot Server')) {
            $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="rebootServer();">Reboot Server</button>;
        }

        if($_SESSION['user']->hasPermission('Update Server')) {
            $this->settings['views']['body']['headerButtonsRight'][] = <button class="btn btn-primary pull-right" onclick="updateServer();">Update Server</button>;
        }

        
        
        $isDisabled = !$_SESSION['user']->hasPermission('Edit');

        $results = $db->read([
            'servers' => 'S',
            'J.SM.server_mapping' => [
                'SM.serverID' => 'S.id'
            ],
            'J.D.domains' => [
                'D.id' => 'SM.domainID'
            ]
        ], ['D.id', 'D.title', 'D.url'], ['S.id' => $server->id,'D.status' => 1]);

        $domains = <tbody id="domainTableBody"></tbody>;
        
        if($results) {
            foreach($results as $row) {
                $domains->appendChild(<tr>
                    <td>{$row['id']}</td>
                    <td><a href={'/domains/' . $row['id']}>{$row['title']}</a></td>
                    <td><a href={'http://' . $row['url']}>{$row['url']}</a></td>
                    <td><button type="button" disabled={$isDisabled} class="btn btn-default removeDomain pull-right" data-id={$row['id']}><span class="glyphicons remove"></span></button></td>
                </tr>);
            }
        }
        
        
        $this->body = <x:frag>
                        <div id="domainModal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="Add Domain" aria-hidden="true">
                            <div class="modal-dialog">
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
                                        <h4 class="modal-title">Add Domain</h4>
                                    </div>
                                    <div id="domainModalBody" class="modal-body">
                                    </div>
                                    <div class="modal-footer">
                                        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                                        <button type="button" class="btn btn-primary" id="domainModalSaveButton" disabled="disabled">
                                            Add
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div class="row">
                            <div class="container">
                                <h1>Server Details</h1>
                                <div class="row">
                                        <form action="" class="form-horizontal" role="form"> 
                                                <input type="hidden" name="serverID" id="serverID" value={$server->id} />
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverTitle">Title</label>
        
                                                        <div class="col-sm-10">
                                                                <input type="text" disabled={$isDisabled} class="form-control" placeholder="Title"
                                                                            data-orgval={$server->title}
                                                                            id="serverTitle" required="required" maxlength="50" value={$server->title} />
                                                        </div>
                                                </div>
        
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverIP">IP</label>
        
                                                        <div class="col-sm-10">
                                                                <input type="text" disabled={$isDisabled} class="form-control" placeholder="IP" id="serverIP"
                                                                            data-orgval={long2ip($server->ip)}
                                                                            required="required" value={long2ip($server->ip)} />
                                                        </div>
                                                </div>
        
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverStatus">Status</label>
        
                                                        <div class="col-sm-10">
                                                                <span class="serverStatusIcon glyphicons circle_question_mark pull-right" data-id={$server->id}></span>
                                                        </div>
                                                </div>
                                                
            
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverUpdates">Updates</label>
        
                                                        <div class="col-sm-10">
                                                                <input type="number" disabled="disabled" class="form-control" placeholder="Updates" id="serverUpdates" value={$server->updates} />
                                                        </div>
                                                </div>
            
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverSecurityUpdates">Security Updates</label>
        
                                                        <div class="col-sm-10">
                                                                <input type="number" disabled="disabled" class="form-control" placeholder="Updates" id="serverSecurityUpdates" value={$server->securityUpdates} />
                                                        </div>
                                                </div>
            
                                                <div class="form-group">
                                                        <label class="col-sm-2 control-label" for="serverRebootRequired">Reboot Required</label>
        
                                                        <div class="col-sm-10">
                                                                <span class="pull-right">{$server->rebootRequired ? 'Yes' : 'No'}</span>
                                                        </div>
                                                </div>
        
                                                <div class="form-group">
                                                        <div id="alertBox"></div>
                                                        <div class="col-sm-2"></div>
                                                        <div class="col-sm-10 text-right">
                                                            <button type="button" disabled={$isDisabled} class="btn btn-primary" onclick="updateForm();">Update Server</button>
                                                        </div>
                                                </div>
                                        </form>
                                </div>
                            </div>
                        </div>
                        <div class="row">        
                            <div class="container">
                                <h2>Domains</h2>
                                <div class="row">
                                    <div class="table-responsive">
                                        <table class="table table-bordered table-striped table-hover" id="domainsTable">
                                            <thead>
                                                <tr>
                                                    <th>ID</th>
                                                    <th>Title</th>
                                                    <th>URL</th>
                                                    <th>Remove</th>
                                                </tr>
                                            </thead>
                                            {$domains}
                                        </table>
                                        <button type="button" disabled={$isDisabled} class="btn btn-primary pull-right" onclick="addDomain();">Add Domain</button>
                                    </div>
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
                                    <div id="serverHistoryApplicationResponseTime" class="chart forceGPU noselect">
                                        <div class="spinner">
                                          <div class="rect1"></div>
                                          <div class="rect2"></div>
                                          <div class="rect3"></div>
                                          <div class="rect4"></div>
                                          <div class="rect5"></div>
                                        </div>
                                    </div>
                                </div>
        
                                <div class="row">
                                    <div class="col-lg-12">
                                        <div id="serverHistoryUsage" class="chart forceGPU noselect">
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
                                    <div class="col-lg-4">
                                        <div id="serverHistoryApplicationRPM" class="chart forceGPU noselect">
                                            <div class="spinner">
                                              <div class="rect1"></div>
                                              <div class="rect2"></div>
                                              <div class="rect3"></div>
                                              <div class="rect4"></div>
                                              <div class="rect5"></div>
                                            </div>
                                        </div>
                                    </div>
        
                                    <div class="col-lg-4">
                                        <div id="serverHistoryApplicationQPM" class="chart forceGPU noselect">
                                            <div class="spinner">
                                              <div class="rect1"></div>
                                              <div class="rect2"></div>
                                              <div class="rect3"></div>
                                              <div class="rect4"></div>
                                              <div class="rect5"></div>
                                            </div>
                                        </div>
                                    </div>
        
                                    <div class="col-lg-4">
                                        <div id="serverHistoryApplicationAVGTimeCPUBound" class="chart forceGPU noselect">
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
                                    <div class="col-lg-4">
                                        <div id="serverHistoryIOWait" class="chart forceGPU noselect">
                                            <div class="spinner">
                                              <div class="rect1"></div>
                                              <div class="rect2"></div>
                                              <div class="rect3"></div>
                                              <div class="rect4"></div>
                                              <div class="rect5"></div>
                                            </div>
                                        </div>
                                    </div>
        
                                    <div class="col-lg-4">
                                        <div id="serverHistoryTPS" class="chart forceGPU noselect">
                                            <div class="spinner">
                                              <div class="rect1"></div>
                                              <div class="rect2"></div>
                                              <div class="rect3"></div>
                                              <div class="rect4"></div>
                                              <div class="rect5"></div>
                                            </div>
                                        </div>
                                    </div>
        
                                    <div class="col-lg-4">
                                        <div id="serverHistoryNetworkTraffic" class="chart forceGPU noselect">
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
                        </div>
                        
                      </x:frag>;
        
		return 1;
	}
}
