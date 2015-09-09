<?hh
namespace HCPublic;

class DataPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'dataTable' => true
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
       
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data</h1>
                    <div class="col-lg-4">
                            <a class="btn btn-default" style="display: block; " href="/data/imports"><span class="glyphicons cloud-upload" style="display: block; font-size: 48px;"></span>Import</a>
                    </div>
                    <div class="col-lg-4">
                            <a class="btn btn-default" style="display: block;" href="/data/exports"><span class="glyphicons cloud-download" style="display: block; font-size: 48px;"></span>Export</a>
                    </div>
                    <div class="col-lg-4">
                            <a class="btn btn-default" style="display: block;" href="/data/templates"><span class="glyphicons wrench" style="display: block; font-size: 48px;"></span>Templates</a>
                    </div>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
