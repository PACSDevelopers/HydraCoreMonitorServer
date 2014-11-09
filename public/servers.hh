<?hh
namespace HCPublic;

class ServersPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Servers',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'serverTable' => true
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

	public function init() {
        if($_SESSION['user']->hasPermission('Create')) {
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/servers/create">Create Server</a>];
        }
		
		$db = new \HC\DB();
        
		$columns = ['ID' => 'id', 'Title' => 'title', 'IP' => 'ip'];
        $serversHeader = <tr></tr>;
		foreach($columns as $key => $column) {
            $serversHeader->appendChild(<th>{$key}</th>);
		}
        $serversHeader->appendChild(<th>Status</th>);
        
        $serversBody = <tbody></tbody>;
        
        $result = $db->read('servers', array_values($columns), ['status' => 1]);
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $serversRow = <tr></tr>;
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $serversRow->appendChild(<td><a href={'/servers/' . $row['id']}>{$value}</a></td>);
                    } else if($key2 === 'url') {
                        $serversRow->appendChild(<td><a href={'http://' . $value}>{$value}</a></td>);
                    } else if($key2 === 'ip') {
                        $serversRow->appendChild(<td>{long2ip($value)}</td>);
                    } else {
                        $serversRow->appendChild(<td>{$value}</td>);
                    }
                }
                $serversRow->appendChild(<td><span class="serverStatusIcon glyphicons circle_question_mark pull-right" data-id={$row['id']}></span></td>);
                $serversBody->appendChild($serversRow);
            }
        }

		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Severs</h1>
                    <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="serversTable">
                            <thead>
                                {$serversHeader}
                            </thead>
                            {$serversBody}
                        </table>
                    </div>
                </div>
            </div>
        </x:frag>;
        
			return 1;
	}
};
