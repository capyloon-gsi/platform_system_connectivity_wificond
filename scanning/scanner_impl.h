/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WIFICOND_SCANNER_IMPL_H_
#define WIFICOND_SCANNER_IMPL_H_

#include <vector>

#include <android-base/macros.h>
#include <binder/Status.h>

#include "com/android/server/wifi/wificond/BnWifiScannerImpl.h"
#include "wificond/net/netlink_utils.h"
#include "wificond/scanning/scan_utils.h"

namespace android {
namespace wificond {

class ClientInterfaceImpl;
class ScanUtils;

class ScannerImpl : public com::android::server::wifi::wificond::BnWifiScannerImpl {
 public:
  static const uint32_t kFastScanIterations;
  static const uint32_t kSlowScanIntervalMultiplier;

  ScannerImpl(uint32_t interface_index,
              const ScanCapabilities& scan_capabilities,
              const WiphyFeatures& wiphy_features,
              ClientInterfaceImpl* client_interface,
              ScanUtils* scan_utils);
  ~ScannerImpl();
  // Get the latest single scan results from kernel.
  ::android::binder::Status getScanResults(
      std::vector<com::android::server::wifi::wificond::NativeScanResult>*
          out_scan_results) override;
  // Get the latest pno scan results from the interface that most recently
  // completed PNO scans
  ::android::binder::Status getPnoScanResults(
      std::vector<com::android::server::wifi::wificond::NativeScanResult>*
          out_scan_results) override;
  ::android::binder::Status scan(
      const ::com::android::server::wifi::wificond::SingleScanSettings&
          scan_settings,
      bool* out_success) override;
  ::android::binder::Status startPnoScan(
      const ::com::android::server::wifi::wificond::PnoSettings& pno_settings,
      bool* out_success) override;
  ::android::binder::Status stopPnoScan(bool* out_success) override;
  ::android::binder::Status abortScan() override;

  ::android::binder::Status subscribeScanEvents(
      const ::android::sp<::com::android::server::wifi::wificond::IScanEvent>& handler) override;
  ::android::binder::Status unsubscribeScanEvents() override;
  ::android::binder::Status subscribePnoScanEvents(
      const ::android::sp<::com::android::server::wifi::wificond::IPnoScanEvent>& handler)
      override;
  ::android::binder::Status unsubscribePnoScanEvents() override;
  void Invalidate();

 private:
  bool CheckIsValid();
  void OnScanResultsReady(uint32_t interface_index, bool aborted,
                          std::vector<std::vector<uint8_t>>& ssids,
                          std::vector<uint32_t>& frequencies);
  void OnSchedScanResultsReady(uint32_t interface_index, bool scan_stopped);
  void LogSsidList(std::vector<std::vector<uint8_t>>& ssid_list,
                   std::string prefix);
  bool StartPnoScanDefault(
      const ::com::android::server::wifi::wificond::PnoSettings& pno_settings);
  bool StopPnoScanDefault();
  void ParsePnoSettings(
      const ::com::android::server::wifi::wificond::PnoSettings& pno_settings,
      std::vector<std::vector<uint8_t>>* scan_ssids,
      std::vector<std::vector<uint8_t>>* match_ssids,
      std::vector<uint32_t>* freqs, std::vector<uint8_t>* match_security);
  SchedScanIntervalSetting GenerateIntervalSetting(
    const ::com::android::server::wifi::wificond::PnoSettings& pno_settings) const;

  // Boolean variables describing current scanner status.
  bool valid_;
  bool scan_started_;
  bool pno_scan_started_;
  ::com::android::server::wifi::wificond::PnoSettings pno_settings_;

  const uint32_t interface_index_;

  // Scanning relevant capability information for this wiphy/interface.
  ScanCapabilities scan_capabilities_;
  WiphyFeatures wiphy_features_;

  ClientInterfaceImpl* client_interface_;
  ScanUtils* const scan_utils_;
  ::android::sp<::com::android::server::wifi::wificond::IPnoScanEvent> pno_scan_event_handler_;
  ::android::sp<::com::android::server::wifi::wificond::IScanEvent> scan_event_handler_;

  DISALLOW_COPY_AND_ASSIGN(ScannerImpl);
};

}  // namespace wificond
}  // namespace android

#endif  // WIFICOND_SCANNER_IMPL_H_
