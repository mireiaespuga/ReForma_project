CREATE TABLE IF NOT EXISTS `artist1` (
  `RowName` int(11) NOT NULL AUTO_INCREMENT,
  `MaterialName` varchar(50) NOT NULL,
  `UMaterialMatch` varchar(50) DEFAULT NULL,
  `FatherName` varchar(50) DEFAULT NULL,
  `TextureNames` varchar(500) DEFAULT NULL,
  `ScalarParamValues` varchar(500) DEFAULT NULL,
  `VectorParamValues` varchar(500) DEFAULT NULL,
  PRIMARY KEY (`RowName`),
  UNIQUE KEY `MaterialName_UNIQUE` (`MaterialName`)
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=latin1;
GRANT SELECT, UPDATE, DELETE, INSERT ON artist1 to `default_user`@`%`; 