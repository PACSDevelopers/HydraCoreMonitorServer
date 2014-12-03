<?hh
namespace HCMS;

class Export extends \HC\Core
{    
    protected $db;
    protected $data = [];

    public function  __construct($export = [])
    {
        // Parse global / local options
        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        
        $this->db = new \HC\DB();
        $tempData = $this->db->read('data_exports', [], $export);
        if($tempData) {
            $this->data = $tempData[0];
        }
    }

    public function checkExists() {
        if(!empty($this->data)) {
            return true;
        }

        return false;
    }

    public function run($databaseID, $schema, $templateID) {
        $result = $this->db->query('SELECT
                                        `DTT`.`id`, `DTT`.`name`
                                    FROM `data_export_tables` `DET`
                                    LEFT JOIN `data_template_tables` `DTT` ON (`DTT`.`id` = `DET`.`tableID`)
                                    WHERE
                                      `DET`.`exportID` = ?;', [$this->id]);
        if($result) {
            $desiredTables = [];
            foreach($result as $table) {
                $desiredTables[$table['id']] = $table['name'];
            }

            $result = null;
            unset($result);

            $template = new \HCMS\Template(['id' => $templateID]);

            $fullTemplate = $template->load();

            $desiredTables = $this->getDesiredTables($fullTemplate, $desiredTables);

            $templateMap = [];
            foreach($fullTemplate as $key => $templateTable) {
                $templateMap[$templateTable['id']] = $key;
            }

            $columnMap = [];
            foreach($fullTemplate as $templateKey => $templateTable) {
                $columnMap[$templateTable['id']] = [];
                foreach($templateTable['columns'] as $key => $column) {
                    $columnMap[$templateTable['id']][$column['id']] = $key;
                }
            }

            $relations = $this->getRelations($fullTemplate, $templateMap, $columnMap);
            $tableHeaders = $this->getTableHeaders($fullTemplate);

            $database = new \HCMS\Database(['id' => $databaseID]);
            $connection = $database->getDatabaseConnection($schema);

            if($connection) {
                $exportSize = 0;
                $tableSize = [];
                $selector = [$schema];
                foreach($desiredTables as $id => $name) {
                    $selector[] = $name;
                }

                $result = $connection->query('SELECT
                                                    `IST`.`table_name`,
                                                    `IST`.`data_length`
                                                FROM `information_schema`.`TABLES` `IST`
                                                WHERE `IST`.`table_schema` = ?
                                                 AND `IST`.`table_name` IN(' . rtrim(str_repeat('?,', count($desiredTables)), ',') . ');', $selector);

                if($result) {
                    $connection->disconnect();

                    foreach($result as $row) {
                        $exportSize += $row['data_length'];
                        $tableSize[$row['table_name']] = $row['data_length'];
                    }

                    $exportData = [];

                    $done = 0;
                    foreach($desiredTables as $id => $name) {
                        $columns = [];
                        foreach($fullTemplate[$templateMap[$id]]['columns'] as $key => $column) {
                            $columns[] = $column['name'];
                        }

                        $connection->connect();
                        $data = $connection->read($name, $columns);
                        $connection->disconnect();

                        if($data) {
                            $data = $this->processRawExport([$id => $data], $fullTemplate, $relations, $tableHeaders);
                        } else {
                            $data = $this->processRawExport([$id => []], $fullTemplate, $relations, $tableHeaders);
                        }

                        $data = json_encode($data[$id]);
                        $hash = md5($data);
                        file_put_contents(HC_LOCATION . '/assets/' . $hash, $data);

                        $exportData[$fullTemplate[$templateMap[$id]]['alias']] = $hash;

                        $data = null;
                        unset($data);
                        $columns = null;
                        unset($columns);

                        $done += $tableSize[$name];
                        $this->db->update('data_exports', ['id' => $this->id], ['progress' => floor(100 - ($exportSize - $done) / $exportSize * 100)]);
                    }


                    if($exportData) {
                        foreach($exportData as $key => $value) {
                            $exportData[$key] = json_decode(file_get_contents(HC_LOCATION . '/assets/' . $value), true);
                            unlink(HC_LOCATION . '/assets/' . $value);
                        }

                        $exportData = json_encode($exportData);
                        $hash = md5($exportData);
                        file_put_contents(HC_LOCATION . '/assets/' . $hash, $exportData);
                        $this->db->update('data_exports', ['id' => $this->id], ['progress' => 100, 'status' => 3, 'hash' => $hash]);
                        return true;
                    }

                    return -4;
                }

                return -3;
            }

            return -2;
        }


        return false;
    }

    protected function processRawExport($exportData, $fullTemplate, $relations, $tableHeaders) {
        $exportData = $this->applyRelations($exportData, $relations);
        $exportData = $this->applyTableHeaders($exportData, $tableHeaders);
        $exportData = $this->flattenData($exportData);

        return $exportData;
    }

    protected function getTableHeaders($fullTemplate) {
        $tableHeaders = [];
        foreach($fullTemplate as $templateKey => $templateTable) {
            $tableHeaders[$templateTable['id']] = [];
            foreach($templateTable['columns'] as $column) {
                $tableHeaders[$templateTable['id']][] = $column['alias'];
            }
        }

        return $tableHeaders;
    }

    protected function getRelations($fullTemplate, $templateMap, $columnMap) {
        $relations = [];
        foreach($fullTemplate as $templateKey => $templateTable) {
            $relations[$templateTable['id']] = [];
            foreach($templateTable['columns'] as $column) {
                if($column['relationTable'] !== '0' && $column['relationColumn'] !== '0') {
                    $tableKey = $templateMap[$column['relationTable']];
                    $columnKey = $columnMap[$column['relationTable']][$column['relationColumn']];

                    $relations[$templateTable['id']][$column['name']] = strtoupper(str_replace(' ', '_', $fullTemplate[$tableKey]['alias'] . '_' . $fullTemplate[$tableKey]['columns'][$columnKey]['alias']));
                }
            }
        }

        return $relations;
    }

    protected function applyRelations($exportData, $relations) {

        foreach($exportData as $id => $rows) {
            foreach($rows as $rowKey => $row) {
                foreach($row as $name => $value) {
                    if(isset($relations[$id][$name])) {
                        $exportData[$id][$rowKey][$name] = $relations[$id][$name] . '_' . $value;
                    }
                }
            }
        }

        return $exportData;
    }

    protected function applyTableHeaders($exportData, $headers) {
        foreach($exportData as $tableKey => $table) {
            array_unshift($exportData[$tableKey], $headers[$tableKey]);
        }

        return $exportData;
    }

    protected function flattenData($exportData) {
        foreach($exportData as $tableKey => $table) {
            foreach($table as $rowKey => $row) {
                $exportData[$tableKey][$rowKey] = array_values($row);
            }
        }

        return $exportData;
    }

    protected function getDesiredTables($templateData, $desiredTables, $realDesiredTables = []) {
        $shouldRecurse = false;

        // Parse tables
        foreach($templateData as $key => $templateTable) {
            if(isset($desiredTables[$templateTable['id']]) && !isset($realDesiredTables[$templateTable['id']])) {
                $realDesiredTables[$templateTable['id']] = $templateTable['name'];
            }
        }

        // Parse related columns
        foreach($templateData as $key => $templateTable) {
            foreach($templateTable['columns'] as $column) {
                if(!isset($realDesiredTables[$column['relationTable']])) {
                    foreach($templateData as $key => $value) {
                        if($value['id'] === $column['relationTable']) {
                            $shouldRecurse = true;
                            $realDesiredTables[$column['relationTable']] = $value['name'];
                            break 2;
                        }
                    }
                }
            }
        }


        if($shouldRecurse) {
            return $this->getDesiredTables($templateData, array_values($realDesiredTables), $realDesiredTables);
        }

        return $realDesiredTables;
    }
    
    public function __set($key, $value)
    {
        $this->data[$key] = $value;
        return true;
    }

    function __get($key) {
        if (array_key_exists($key, $this->data)) {
            return $this->data[$key];
        }

        return false;
    }

    public function __isset($key)
    {
        return isset($this->data[$key]);
    }

    public function __unset($key)
    {
        unset($this->data[$key]);
    }
}
