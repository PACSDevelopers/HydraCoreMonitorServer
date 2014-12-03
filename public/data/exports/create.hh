<?hh
namespace HCPublic\Data\Exports;

class CreatePage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Exports - Create',
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

        $templates = <select id="templateSelect" class="form-control"><option value="0" selected="selected" disabled="disabled">Please select a template</option></select>;
        $databases = <select id="databaseSelect" class="form-control"><option value="0" selected="selected" disabled="disabled">Please select a database</option></select>;
        
        $db = new \HC\DB();
        $result = $db->read('data_templates', ['id', 'title'], ['status' => 1]);
        
        if($result) {
            foreach($result as $row) {
                $templates->appendChild(<option value={$row['id']}>{$row['title']}</option>);
            }
        }

        $result = $db->read('databases', ['id', 'title'], ['status' => 1]);

        if($result) {
            foreach($result as $row) {
                $databases->appendChild(<option value={$row['id']}>{$row['title']}</option>);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Exports - Create</h1>
                </div>
                <div class="row">
                    <form action="" class="form-horizontal" role="form">
                        <div class="form-group">
                            <label class="col-sm-2 control-label" for="templateSelect">Template</label>
                            <div class="col-sm-10">
                                    {$templates}
                            </div>
                        </div>
                        <div class="form-group">
                            <label class="col-sm-2 control-label" for="databaseSelect">Database</label>
                            <div class="col-sm-10">
                                    {$databases}
                            </div>
                        </div>
                        <div class="form-group">
                            <label class="col-sm-2 control-label" for="schemaSelect">Schema</label>
                            <div class="col-sm-10">
                                <select id="schemaSelect" class="form-control"><option value="0" selected="selected" disabled="disabled">Please select a schema</option></select>
                            </div>
                        </div>
                    </form>
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
