<?hh
namespace HCPublic\Data\Exports;

class ExportPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Exports - ',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'forms'      => true,
                                    'data/exports/export' => true
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

	public function init($GET, $POST) {

        $database = new \HCMS\Database(['id' => $GET['id']]);

        if(!$database->checkExists()) {
            return 404;
        }

        $this->settings['views']['header']['pageName'] .= $database->title;
        
        $schemaList = <tbody></tbody>;
        $schemas = $database->getSchemas();
        
        if($schemas) {
            foreach($schemas as $schema) {
                $schemaList->appendChild(<tr><td><a href={'/data/exports/' . $database->id . '/' . $schema}>{$schema}</a></td></tr>);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Exports - {$database->title}</h1>
                    
                </div>
                <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="databasesTable">
                            <thead>
                                <tr>
                                    <th>Name</th>
                                </tr>
                            </thead>
                            {$schemaList}
                        </table>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
