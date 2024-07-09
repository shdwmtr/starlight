export const FormatBytes = (inputBytes: number, decimals = 2) => {
    if (inputBytes === 0) {
        return '0 Bytes';
    }

    const kilobyteSize = 1024;
    const sizeUnitMap = ['Bytes', 'KB', 'MB', 'GB']; // Added 'GB' to the sizeUnitMap
    let sizeIndex = Math.floor(Math.log(inputBytes) / Math.log(kilobyteSize));

    // Adjusting for GB if sizeIndex goes beyond the defined units
    if (sizeIndex >= sizeUnitMap.length) {
        sizeIndex = sizeUnitMap.length - 1;
    }

    return parseFloat((inputBytes / Math.pow(kilobyteSize, sizeIndex))
        .toFixed(decimals < 0 ? 0 : decimals)) + ' ' + sizeUnitMap[sizeIndex];
}