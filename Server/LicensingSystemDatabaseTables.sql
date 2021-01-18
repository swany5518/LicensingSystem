--
-- Database: `LicensingSystem`
--

-- --------------------------------------------------------

--
-- Table structure for table `client`
--

CREATE TABLE `client` (
  `ClientID` varchar(36) NOT NULL,
  `Username` varchar(24) NOT NULL,
  `PasswordHash` varchar(36) NOT NULL,
  `HardwareID` varchar(36) NOT NULL,
  `IpAddress` varchar(32) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `license`
--

CREATE TABLE `license` (
  `LicenseID` varchar(36) NOT NULL,
  `ClientID` varchar(36) NOT NULL,
  `ProductID` varchar(36) NOT NULL,
  `LicenseStart` int(11) NOT NULL,
  `LicenseEnd` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `licenseKey`
--

CREATE TABLE `licenseKey` (
  `LicenseKey` varchar(36) NOT NULL,
  `ProductID` varchar(36) NOT NULL,
  `LicenseDays` int(11) NOT NULL,
  `Vendor` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `product`
--

CREATE TABLE `product` (
  `ProductID` varchar(36) NOT NULL,
  `ProductName` varchar(255) NOT NULL,
  `Status` varchar(255) DEFAULT NULL,
  `ServerFilepath` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `redeemedKeys`
--

CREATE TABLE `redeemedKeys` (
  `LicenseKey` varchar(36) NOT NULL,
  `ClientID` varchar(36) NOT NULL,
  `ProductID` varchar(36) NOT NULL,
  `LicenseDays` int(11) NOT NULL,
  `Vendor` varchar(255) NOT NULL,
  `RedemptionTime` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `restrictions`
--

CREATE TABLE `restrictions` (
  `RestrictionID` varchar(36) NOT NULL,
  `ClientID` varchar(36) NOT NULL,
  `ProductID` varchar(36) NOT NULL,
  `RestrictionStart` int(11) NOT NULL,
  `RestrictionEnd` int(11) NOT NULL,
  `Reason` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `client`
--
ALTER TABLE `client`
  ADD PRIMARY KEY (`ClientID`),
  ADD UNIQUE KEY `ClientID` (`ClientID`);

--
-- Indexes for table `license`
--
ALTER TABLE `license`
  ADD PRIMARY KEY (`LicenseID`);

--
-- Indexes for table `licenseKey`
--
ALTER TABLE `licenseKey`
  ADD PRIMARY KEY (`LicenseKey`);

--
-- Indexes for table `product`
--
ALTER TABLE `product`
  ADD PRIMARY KEY (`ProductID`);

--
-- Indexes for table `redeemedKeys`
--
ALTER TABLE `redeemedKeys`
  ADD PRIMARY KEY (`LicenseKey`);

--
-- Indexes for table `restrictions`
--
ALTER TABLE `restrictions`
  ADD PRIMARY KEY (`RestrictionID`);
COMMIT;
