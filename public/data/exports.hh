<?hh
namespace HCPublic\Data;

class ExportsPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Exports',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'data/exports' => true
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

        $db = new \HC\DB();

        $columns = ['ID' => 'id', 'Title' => 'title'];
        $databasesHeader = <tr></tr>;
		foreach($columns as $key => $column) {
            $databasesHeader->appendChild(<th>{$key}</th>);
		}
        
        $databasesBody = <tbody></tbody>;
        
        $result = $db->read('databases', array_values($columns), ['status' => 1]);
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $databasesRow = <tr></tr>;
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $databasesRow->appendChild(<td><a href={'/data/exports/' . $row['id']}>{$value}</a></td>);
                    } else if($key2 === 'url') {
                        $databasesRow->appendChild(<td><a href={'http://' . $value}>{$value}</a></td>);
                    } else if($key2 === 'extIP' || $key2 === 'intIP') {
                        $databasesRow->appendChild(<td>{long2ip($value)}</td>);
                    } else {
                        $databasesRow->appendChild(<td>{$value}</td>);
                    }
                }
                $databasesBody->appendChild($databasesRow);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Exports</h1>
                    <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="databasesTable">
                            <thead>
                                {$databasesHeader}
                            </thead>
                            {$databasesBody}
                        </table>
                    </div>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
