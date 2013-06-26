#ifndef SOLARFLARE_SF_UTILS_H
#define SOLARFLARE_SF_UTILS_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

/// File encorporates classes that should not be subclassed for
/// platform-specific provider implementation.

// Macro to disable compiler warning about an unused parameter
#define UNUSED(_x) (void )(_x)

namespace solarflare 
{
    // fixme: what do we actually need here?
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;

    /// @brief Detailed version info for SW elements
    class VersionInfo {
        unsigned vmajor; //< Major version
        unsigned vminor; //< Minor version
        unsigned revisionNo; //< Revision number
        unsigned buildNo; //< Build number
        String versionStr; //< Version string
        Datetime dateReleased; //< Release date
    public:
        /// A value for missing version component
        static const unsigned unknown;

        /// Construct version data from components
        VersionInfo(unsigned mj, unsigned mn, 
                    unsigned rev = unknown, 
                    unsigned bno = unknown,
                    const String& vstr = "", const Datetime& rd = Datetime()) :
            vmajor(mj), vminor(mn), revisionNo(rev), buildNo(bno),
            versionStr(vstr), 
            dateReleased(rd) {}

        /// Construct version data with all fields set to unknown values
        VersionInfo() :
            vmajor(unknown), vminor(unknown), revisionNo(unknown), 
            buildNo(unknown) {}

        /// Construct version data by parsing a version string.
        /// It must have the form 'major.minor[.revision[.build]] rest'.
        /// Missing fields (incl. release date) are set to unknown.
        /// The versionStr field itself is set to @p s
        VersionInfo(const char *s);

        /// @return true iff two version data are equivalent
        bool operator == (const VersionInfo& v) const;

        /// @return true iff two version data are not equivalent
        bool operator != (const VersionInfo& v) const
        {
            return !(*this == v);
        }

        /// @return major version number
        unsigned major() const { return vmajor; }
        /// @return minor version number
        unsigned minor() const { return vminor; }
        /// @return revision number (3rd numeric version component)
        unsigned revision() const { return revisionNo; }
        /// @return build number (4th numeric version component)
        unsigned build() const{ return buildNo; }
        /// @return the release date (zero point if not known)
        Datetime releaseDate() const {  return dateReleased; }

        /// @return String rep of the version info
        /// If versionStr is non-empty, it is simply returned, otherwise
        /// the returned string is 'major.minor[.revision[.build]]'
        String string() const;

        /// @return true iff all fields are set to 'unknown' values
        bool isUnknown() const;
    };

    inline bool VersionInfo::operator == (const VersionInfo& v) const
    {
        return (vmajor == v.vmajor &&
                vminor == v.vminor &&
                revisionNo == v.revisionNo &&
                buildNo == v.buildNo &&
                versionStr == v.versionStr &&
                dateReleased == v.dateReleased);
    }

    inline bool VersionInfo::isUnknown() const
    {
        return (vmajor == unknown &&
                vminor == unknown &&
                revisionNo == unknown &&
                buildNo == unknown &&
                versionStr.size() == 0 &&
                dateReleased == Datetime());
    }

    /// @brief PCI address. If deviceId is set to a known value, the address
    /// is a BDF; otherwise it is assumed to be an ARI-style address.  fnId
    /// is set to unknown for slot addresses
    class PCIAddress {
        unsigned pciDomain;
        unsigned pciBus;
        unsigned deviceId;
        unsigned fnId;
    public:
        /// Unknown id (same for all parts of the address).
        static const unsigned unknown;

        /// Constructor. For PCI slot we don't need function part and can fill in either domain+bus or ARI DeviceID.
        ///
        /// @param dom Domain
        /// @param b   Bus
        /// @param dev Device
        /// @param fn  Function
        PCIAddress(unsigned dom, unsigned b, 
                   unsigned dev = unknown, unsigned fn = unknown) :
            pciDomain(dom), pciBus(b), deviceId(dev), fnId(fn) {}

        // Accessors
        unsigned domain() const { return pciDomain; }
        unsigned bus() const { return pciBus; }
        unsigned device() const { return deviceId; }
        unsigned fn() const { return fnId; }

        /// @return Are we dealing with ARI-style address?
        bool isARI() const { return deviceId == unknown && fnId != unknown; }

        /// @return Is the address pointing to a slot?
        bool isSlot() const { return fnId == unknown; }

        /// Return a new  PCI address with a function id set to @p f
        /// Useful to construct device PCI address from slot address
        PCIAddress fn(unsigned f) const
        {
            return PCIAddress(pciDomain, pciBus, deviceId, f);
        }
        
    };

    /// @brief NIC's vital product data (VPD) representation
    /// The set of fields is defined according to SF-108427-TC-2
    /// (IBM's requirements).
    class VitalProductData {
        String uuid;
        String manufac;
        String serialNo;
        String partNo;
        String modelId;
        String fruNo;
    public:
        VitalProductData(const String& id, const String& manf, 
                         const String& sno, const String& pno,
                         const String& m, const String& fru) :
            uuid(id), manufac(manf), serialNo(sno), partNo(pno),
            modelId(m), fruNo(fru) {}
        VitalProductData() {}
        const String& id() const { return uuid; }
        /// @return Manufacturer field, or calls
        /// System::target.manufacturer() if the former is empty.
        const String& manufacturer() const;
        const String& serial() const { return serialNo; }
        const String& part() const { return partNo; }
        const String& model() const { return modelId; }
        const String& fru() const { return fruNo; }
    };

    /// MAC address data structure.
    ///
    /// fixme: constant address length, ETH_ALEN is not portable.
    struct MACAddress {
        unsigned char address[6]; //< Address

        /// We need this because C++ (prior to 2011 edition)
        /// does not allow compound initializers in many contexts
        MACAddress(unsigned a0, unsigned a1, unsigned a2,
                   unsigned a3, unsigned a4, unsigned a5);
        MACAddress();
        String string() const;
    };

    inline MACAddress::MACAddress(unsigned a0, unsigned a1, unsigned a2,
                           unsigned a3, unsigned a4, unsigned a5)
    {
        address[0] = a0;
        address[1] = a1;
        address[2] = a2;
        address[3] = a3;
        address[4] = a4;
        address[5] = a5;
    }
    inline MACAddress::MACAddress()
    {
        memset(address, 0, sizeof(address));
    }
} // namespace

#endif // SOLARFLARE_SF_UTILS_H
