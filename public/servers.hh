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
		$serversTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'serversTable']);
		$serversTable->openHeader();
		$serversTable->openRow();
		foreach($columns as $key => $column) {
            $serversTable->column(['value' => $key]);
        }
        $serversTable->column(['value' => 'Status']);
		$serversTable->closeRow();
		$serversTable->closeHeader();
        
        $result = $db->read('servers', array_values($columns), ['status' => 1]);
        $serversTable->openBody();
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $serversTable->openRow();
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $serversTable->column(['value' => <a href={'/servers/' . $row['id']}>{$value}</a> ]);
                    } else if($key2 === 'url') {
                        $serversTable->column(['value' => <a href={$value}>{$value}</a>]);
                    } else if($key2 === 'ip') {
                        $serversTable->column(['value' => long2ip($value)]);
                    } else {
                        $serversTable->column(['value' => $value]);
                    }
                }
                $serversTable->column(['value' => <span class="serverStatusIcon glyphicons circle_question_mark pull-right" data-id={$row['id']}></span>]);
                $serversTable->closeRow();
            }
        }
        
        $serversTable->closeBody();

		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Severs</h1>
                    <div class="table-responsive">
                        {$serversTable}
                    </div>
                </div>
            </div>
        </x:frag>;
        
			return 1;
	}
};
