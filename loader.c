// This will load a lorito bytecode file into a lorito codeseg

// Since this is temporary, and we'll end up throwing it away in favor of
//  integrating with parrot's packfile format, this will be real simple.
//
// Integer: segment type (0 = code, 1 = data)
//   Integer: Size of segement name
//   String: segment name, null terminated
//   Integer: Count (in 8 bytes, so a count of 1 == 8 bytes)
//   Data
