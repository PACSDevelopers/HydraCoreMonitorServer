<?hh
namespace HCPublic\Data\Exports;

class SchemaExportPage extends \HC\Page {

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

        $this->settings['views']['header']['pageName'] .= $database->title . ' - ' . ucfirst($GET['name']);

        $templates = <select id="templateSelect" class="form-control"><option value="0" selected="selected" disabled="disabled">Please select a template</option></select>;
        
        $db = new \HC\DB();
        $result = $db->read('data_templates', ['id', 'title'], ['status' => 1]);
        
        if($result) {
            foreach($result as $row) {
                $templates->appendChild(<option value={$row['id']}>{$row['title']}</option>);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Exports - {$database->title . ' - ' . ucfirst($GET['name'])}</h1>
                </div>
                <div class="row">
                    <div class="form-group">
                        <input type="hidden" id="databaseID" name="databaseID" value={$GET['id']} />
                        {$templates}
                    </div>
                </div>
                <div class="row" id="template">
                    <ul class="list-group row flexbox-container" id="tableRowList"></ul>
                    <div class="cleafix"></div>
                    <div id="alertBox"></div>
                    <div class="cleafix"></div>
                    <button class="btn btn-lg btn-primary btn-block" type="button" onclick="runExport();">Schedule Export</button>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
