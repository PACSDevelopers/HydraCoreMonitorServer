<?hh
namespace HCPublic\Downloads;

class ExportsPage extends \HC\Page {

    protected $settings = [
        'authentication' => true
    ];

    protected $formats = ['JSON', 'CSV', 'XLS', 'XLSX', 'YAML'];

    public function init($GET = [], $POST = []) {
        if($_SESSION['user']->hasPermission('Export')) {
            if(isset($GET['id']) && isset($GET['format']) && in_array($GET['format'], $this->formats, true)) {
                $export = new \HCMS\Export(['id' => $GET['id']]);
                if($export) {
                    if($export->checkExists() && $export->status == 3 && !empty($export->hash)) {
                        $file = HC_LOCATION . '/assets/' . $export->hash;
                        if(file_exists($file)) {
                            $result = $this->downloadFile($GET['id'], $file, $GET['format']);
                            if($result === true) {
                                $this->setRendered(true);
                                return 1;
                            } else {
                                return $result;
                            }
                        }
                    }
                }
            }
        } else {
            return 403;
        }

        return 404;
	}

    protected function downloadFile($id, $file, $format) {
        $data = file_get_contents($file);

        switch($format) {
            case 'CSV':
                return $this->downloadCSV($id, $data);
                break;
            case 'XLS':
                return $this->downloadXLS($id, $data);
                break;
            case 'XLSX':
                return $this->downloadXLSX($id, $data);
                break;
            case 'XML':
                return $this->downloadXML($id, $data);
                break;
            case 'YAML':
                return $this->downloadYAML($id, $data);
                break;
            default:
                return $this->downloadJSON($id, $data);
                break;
        }
    }


    protected function downloadCSV($id, $data) {
        $data = json_decode($data, true);

        if(!is_dir(HC_TMP_LOCATION . '/exports')) {
            mkdir(HC_TMP_LOCATION . '/exports', 0777);
        }

        if(!is_dir(HC_TMP_LOCATION . '/exports/' . $id)) {
            mkdir(HC_TMP_LOCATION . '/exports/' . $id, 0777);
        }

        foreach($data as $key => $rows) {
            $fp = fopen(HC_TMP_LOCATION . '/exports/' . $id . '/' . $key . '.csv', 'w');

            fputcsv($fp, []);

            foreach ($rows as $columns) {
                fputcsv($fp, $columns);
            }

            fclose($fp);

            $data[$key] = null;
        }

        $zip = new \ZipArchive();
        $zip->open(HC_TMP_LOCATION . '/exports/' . $id . '.zip', \ZipArchive::CREATE);
        foreach($data as $key => $rows) {
            $zip->addFile(HC_TMP_LOCATION . '/exports/' . $id . '/' . $key . '.csv', $key . '.csv');
        }

        if (!$zip->status == \ZIPARCHIVE::ER_OK) {
            $returnCode =  404;
        } else {
            $returnCode = true;
            $zip->close();
        }



        foreach($data as $key => $rows) {
            if(file_exists(HC_TMP_LOCATION . '/exports/' . $id . '/' . $key . '.csv')) {
                unlink(HC_TMP_LOCATION . '/exports/' . $id . '/' . $key . '.csv');
            }
        }

        if($returnCode === true) {
            header('Pragma: public');
            header('Content-Type: application/zip');
            header('Content-Length: ' . filesize(HC_TMP_LOCATION . '/exports/' . $id . '.zip'));
            header('ETag: '. md5_file(HC_TMP_LOCATION . '/exports/' . $id . '.zip') .'');
            header('Cache-Control: max-age=' . strtotime('+1 year', time()));
            header('Expires: ' . strtotime('+1 year', time()));
            header('Content-Disposition: attachment; filename="' . $id . '.zip"');

            readfile(HC_TMP_LOCATION . '/exports/' . $id . '.zip');
            unlink(HC_TMP_LOCATION . '/exports/' . $id . '.zip');
            rmdir(HC_TMP_LOCATION . '/exports/' . $id);
        }

        return $returnCode;
    }

    protected function downloadXLS($id, $data) {
        $data = json_decode($data, true);

        if(!is_dir(HC_TMP_LOCATION . '/exports')) {
            mkdir(HC_TMP_LOCATION . '/exports', 0777);
        }

        $doc = new \PHPExcel();
        $index = 0;

        foreach($data as $key => $value) {
            if($index) {
                $doc->createSheet($index);
            }
            $doc->setActiveSheetIndex($index);
            $sheet = $doc->getActiveSheet();
            $sheet->setTitle($key);
            $sheet->fromArray($value, null, 'A1');

            $data[$key] = null;

            $cellIterator = $sheet->getRowIterator()->current()->getCellIterator();
            $cellIterator->setIterateOnlyExistingCells(true);

            foreach($cellIterator as $cell) {
                $sheet->getColumnDimension($cell->getColumn())->setAutoSize(true);
            }

            $index++;
        }

        $writer = \PHPExcel_IOFactory::createWriter($doc, 'Excel5');

        $writer->save(HC_TMP_LOCATION . '/exports/' . $id . '.xls');
        $doc->disconnectWorksheets();

        $doc = null;
        $writer = null;
        unset($doc);
        unset($writer);

        header('Pragma: public');
        header('Content-Type: application/vnd.ms-excel');
        header('Content-Length: ' . filesize(HC_TMP_LOCATION . '/exports/' . $id . '.xls'));
        header('ETag: '. md5_file(HC_TMP_LOCATION . '/exports/' . $id . '.xls') .'');
        header('Cache-Control: max-age=' . strtotime('+1 year', time()));
        header('Expires: ' . strtotime('+1 year', time()));
        header('Content-Disposition: attachment; filename="' . $id . '.xls"');

        readfile(HC_TMP_LOCATION . '/exports/' . $id . '.xls');
        unlink(HC_TMP_LOCATION . '/exports/' . $id . '.xls');

        return true;
    }

    protected function downloadXLSX($id, $data) {
        $data = json_decode($data, true);

        if(!is_dir(HC_TMP_LOCATION . '/exports')) {
            mkdir(HC_TMP_LOCATION . '/exports', 0777);
        }

        $doc = new \PHPExcel();
        $index = 0;

        foreach($data as $key => $value) {
            if($index) {
                $doc->createSheet($index);
            }
            $doc->setActiveSheetIndex($index);
            $sheet = $doc->getActiveSheet();
            $sheet->setTitle($key);
            $sheet->fromArray($value, null, 'A1');

            $data[$key] = null;

            $cellIterator = $sheet->getRowIterator()->current()->getCellIterator();
            $cellIterator->setIterateOnlyExistingCells(true);

            foreach($cellIterator as $cell) {
                $sheet->getColumnDimension($cell->getColumn())->setAutoSize(true);
            }

            $index++;
        }

        $writer = \PHPExcel_IOFactory::createWriter($doc, 'Excel2007');

        $writer->save(HC_TMP_LOCATION . '/exports/' . $id . '.xlsx');
        $doc->disconnectWorksheets();

        $doc = null;
        $writer = null;
        unset($doc);
        unset($writer);

        header('Pragma: public');
        header('Content-Type: application/vnd.ms-excel');
        header('Content-Length: ' . filesize(HC_TMP_LOCATION . '/exports/' . $id . '.xlsx'));
        header('ETag: '. md5_file(HC_TMP_LOCATION . '/exports/' . $id . '.xlsx') .'');
        header('Cache-Control: max-age=' . strtotime('+1 year', time()));
        header('Expires: ' . strtotime('+1 year', time()));
        header('Content-Disposition: attachment; filename="' . $id . '.xlsx"');

        readfile(HC_TMP_LOCATION . '/exports/' . $id . '.xlsx');
        unlink(HC_TMP_LOCATION . '/exports/' . $id . '.xlsx');

        return true;
    }

    protected function downloadYAML($id, $data) {
        $data = json_decode($data, true);
        $data = \Spyc::YAMLDump($data);
        header('Pragma: public');
        header('Content-Type: application/x-yaml');
        header('Content-Length: ' . strlen($data));
        header('ETag: '. md5($data) .'');
        header('Cache-Control: max-age=' . strtotime('+1 year', time()));
        header('Expires: ' . strtotime('+1 year', time()));
        header('Content-Disposition: attachment; filename="' . $id . '.yaml"');
        print $data;
        return true;
    }

    protected function downloadJSON($id, $data) {
        header('Pragma: public');
        header('Content-Type: application/json');
        header('Content-Length: ' . strlen($data));
        header('ETag: '. md5($data) .'');
        header('Cache-Control: max-age=' . strtotime('+1 year', time()));
        header('Expires: ' . strtotime('+1 year', time()));
        header('Content-Disposition: attachment; filename="' . $id . '.json"');
        print $data;
        return true;
    }
}
