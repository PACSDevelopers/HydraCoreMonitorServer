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
									'serverForm' => true
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
            $this->settings['views']['body']['headerButtonsRight'] = [<button class="btn btn-primary" onclick="deleteServer();">Delete Server</button>];
        }

        $isDisabled = !$_SESSION['user']->hasPermission('Edit');
        
        $this->body = <x:frag>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                        <div class="row col-lg-8 col-md-12 col-sm-12">
                            <h1>Server Details</h1>
                            <div class="row">
                                    <form action="" class="form-horizontal" role="form"> 
                                            <input type="hidden" name="serverD" id="serverID" value={$server->id} />
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
                                                    <div id="alertBox"></div>
                                                    <div class="col-sm-2"></div>
                                                    <div class="col-sm-10 text-right">
                                                        <button type="button" disabled={$isDisabled} class="btn btn-primary" onclick="updateForm();">Update Server</button>
                                                    </div>
                                            </div>
                                    </form>
                            </div>
                        </div>
                        <div class="row col-lg-2 col-md-0 col-sm-0">
                        </div>
                      </x:frag>;
        
		return 1;
	}
}
